#include "Renderer.h"
#include "Engine/Okay/Okay.h"

#include "ContentBrowser.h"
#include "RenderTexture.h"

#include "Engine/Application/Entity.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/MeshComponent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <DirectXMath.h>
#include <utility>

namespace Okay
{
	Renderer::Renderer(RenderTexture* pRenderTarget, ContentBrowser& content)
		:pMeshIL(), pMeshVS(), pDevContext(DX11::getInstance().getDeviceContext()), content(content), pRenderTarget(pRenderTarget)
	{
		OKAY_ASSERT(pRenderTarget, "RenderTarget was nullptr");
		content.addShader(content, "Default");
		content.importFile("../OkayEngine/engine_resources/DefaultTexture.png");
		content.addMaterial(Material::Description()).setName("Default");

		glm::mat4 Identity4x4(1.f);

		DX11::createConstantBuffer(&pMaterialBuffer, nullptr, sizeof(Material::GPUData), false);
		DX11::createConstantBuffer(&pCameraBuffer, nullptr, sizeof(GPUCamera), false);
		DX11::createConstantBuffer(&pWorldBuffer, &Identity4x4, sizeof(glm::mat4), false);
		DX11::createConstantBuffer(&pLightInfoBuffer, nullptr, 16, false);
		DX11::createConstantBuffer(&pShaderDataBuffer, nullptr, sizeof(Shader::GPUData), false);

		expandPointLights();
		expandDirLights();
		createVertexShaders();
		createPixelShaders();

		// Sampler
		{
			ID3D11SamplerState* simp;
			D3D11_SAMPLER_DESC desc{};
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.MinLOD = -FLT_MAX;
			desc.MaxLOD = FLT_MAX;
			desc.MipLODBias = 0.f;
			desc.MaxAnisotropy = 1u;
			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			DX11::getInstance().getDevice()->CreateSamplerState(&desc, &simp);

			pDevContext->PSSetSamplers(0, 1, &simp);
			pDevContext->VSSetSamplers(0, 1, &simp);
			simp->Release();
		}

		meshes.reserve(10);
		pointLights.reserve(10);
		dirLights.reserve(2);

		D3D11_RASTERIZER_DESC rsDesc{};
		rsDesc.FillMode = D3D11_FILL_WIREFRAME;
		rsDesc.CullMode = D3D11_CULL_NONE;
		rsDesc.FrontCounterClockwise = FALSE;
		rsDesc.DepthBias = 0;
		rsDesc.SlopeScaledDepthBias = 0.0f;
		rsDesc.DepthBiasClamp = 0.0f;
		rsDesc.DepthClipEnable = TRUE;
		rsDesc.ScissorEnable = FALSE;
		rsDesc.MultisampleEnable = FALSE;
		rsDesc.AntialiasedLineEnable = FALSE;

		DX11::getInstance().getDevice()->CreateRasterizerState(&rsDesc, &pRSWireFrame);
		OKAY_ASSERT(pRSWireFrame, "Failed to create wireframe RS");

		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		onTargetResize();

		bindNecessities();
	}

	Renderer::~Renderer()
	{
		shutdown();
	}

	void Renderer::submit(const MeshComponent& mesh, const Transform& transform)
	{
		// Remove? std::vector reallocates itself but having control over the amount can be important
		if (meshes.size() == meshes.capacity())
			meshes.reserve(meshes.size() + 10ull);

		meshes.emplace_back(mesh, transform.matrix);
	}

	void Renderer::submit(const PointLight& light, const Transform& transform)
	{
		if (pointLights.size() == pointLights.capacity())
			expandPointLights();

		pointLights.emplace_back(light, transform);
		lightInfo.numPointLights++;
	}

	void Renderer::submit(const DirectionalLight& light, const Transform& transform)
	{
		if (dirLights.size() == dirLights.capacity())
			expandDirLights();
		
		dirLights.emplace_back(light, transform);
		lightInfo.numDirLights++;
	}

	void Renderer::setRenderTexture(RenderTexture* pRenderTexture)
	{
		OKAY_ASSERT(pRenderTexture, "RenderTarget was nullptr");

		pRenderTarget->removeOnResizeCallback(&Renderer::onTargetResize, this);
		pRenderTarget = pRenderTexture;
		pRenderTarget->addOnResizeCallback(&Renderer::onTargetResize, this);

		onTargetResize();
	}

	void Renderer::onTargetResize()
	{
		const glm::ivec2 dims = pRenderTarget->getDimensions();
		viewport.Width = (float)dims.x;
		viewport.Height = (float)dims.y;

		pDevContext->RSSetViewports(1u, &viewport);
	}

	void Renderer::newFrame()
	{
		lightInfo.numPointLights = 0u;
		lightInfo.numDirLights = 0u;
		meshes.clear();
		pointLights.clear();
		dirLights.clear();
	}

	void Renderer::shutdown()
	{
		DX11_RELEASE(pCameraBuffer);
		DX11_RELEASE(pWorldBuffer);
		DX11_RELEASE(pMaterialBuffer);
		DX11_RELEASE(pShaderDataBuffer);

		DX11_RELEASE(pMeshIL);
		DX11_RELEASE(pMeshVS);
		DX11_RELEASE(pRSWireFrame);

		DX11_RELEASE(pLightInfoBuffer);
		DX11_RELEASE(pPointLightBuffer);
		DX11_RELEASE(pPointLightSRV);
		DX11_RELEASE(pDirLightBuffer);
		DX11_RELEASE(pDirLightSRV);

		DX11_RELEASE(pAniVS);
		DX11_RELEASE(pAniIL);
	}

	void Renderer::render(Entity cameraEntity)
	{
		updatePointLightsBuffer();
		updateDirLightsBuffer();
		DX11::updateBuffer(pLightInfoBuffer, &lightInfo, (uint32_t)sizeof(LightInfo));

		// MAKE FUNCTION
		// Calculate viewProjection matrix
		OKAY_ASSERT(cameraEntity.hasComponent<Camera>(), "MainCamera doesn't have a Camera Component");
		const Camera& camera = cameraEntity.getComponent<Camera>();
		Transform& camTransform = cameraEntity.getComponent<Okay::Transform>();
		camTransform.calculateMatrix();
		camData.direction = camTransform.forward();
		camData.pos = camTransform.position;
		camData.viewProjMatrix =  glm::transpose(camera.projectionMatrix *
			glm::lookAtLH(camTransform.position, camTransform.position + camData.direction, camTransform.up()));
		DX11::updateBuffer(pCameraBuffer, &camData, sizeof(GPUCamera));


		// Bind static mesh pipeline
		bindMeshPipeline();

		pDevContext->OMSetRenderTargets(1u, pRenderTarget->getRTV(), *pRenderTarget->getDSV());
		
		ID3D11ShaderResourceView* textures[3] = {};
		size_t i = 0;
		glm::mat4 worldMatrix{};

		// Draw all statis meshes
		for (i = 0; i < meshes.size(); i++)
		{
			const MeshComponent& cMesh = meshes[i].asset;
			const Mesh& mesh = content.getMesh(cMesh.meshIdx);
			const Material& material = content.getMaterial(cMesh.materialIdx);
			const Shader& shader = content.getShader(cMesh.shaderIdx);

			worldMatrix = glm::transpose(meshes[i].transform);

			textures[Material::BASECOLOUR_INDEX] = content.getTexture(material.getBaseColour()).getSRV();
			textures[Material::SPECULAR_INDEX]	 = content.getTexture(material.getSpecular()).getSRV();
			textures[Material::AMBIENT_INDEX]	 = content.getTexture(material.getAmbient()).getSRV();

			DX11::updateBuffer(pWorldBuffer, &worldMatrix, sizeof(glm::mat4));
			DX11::updateBuffer(pMaterialBuffer, &material.getGPUData(), sizeof(Material::GPUData));
			DX11::updateBuffer(pShaderDataBuffer, &shader.getGPUData(), sizeof(Shader::GPUData));

			// IA
			pDevContext->IASetVertexBuffers(0u, Mesh::NumBuffers, mesh.getBuffers(), Mesh::Stride, Mesh::Offset);
			pDevContext->IASetIndexBuffer(mesh.getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0u);

			// VS
			
			// RS
			
			// PS
			shader.bind();
			pDevContext->PSSetShaderResources(0u, 3u, textures);

			// OM

			// Draw
			pDevContext->DrawIndexed(mesh.getNumIndices(), 0u, 0u);

		}
	}

	void Renderer::setWireframe(bool wireFrame)
	{
		pDevContext->RSSetState(wireFrame ? pRSWireFrame : nullptr);
	}

	void Renderer::expandPointLights()
	{
		static const size_t Increase = 5;
		static const size_t GPUPointLightSize = sizeof(PointLight) + sizeof(glm::vec3);
		HRESULT hr = E_FAIL;

		DX11_RELEASE(pPointLightBuffer);
		DX11_RELEASE(pPointLightSRV);

		pointLights.reserve(pointLights.size() + Increase);

		hr = DX11::createStructuredBuffer(&pPointLightBuffer, pointLights.data(), GPUPointLightSize, (uint32_t)pointLights.capacity(), false);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating pointLight structured buffer");

		hr = DX11::createStructuredSRV(&pPointLightSRV, pPointLightBuffer, (uint32_t)pointLights.capacity());
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating pointLight SRV");

		pDevContext->PSSetShaderResources(3, 1, &pPointLightSRV);
	}

	void Renderer::updatePointLightsBuffer()
	{
		D3D11_MAPPED_SUBRESOURCE sub;
		if (FAILED(pDevContext->Map(pPointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub)))
			return;

		char* location = (char*)sub.pData;
		for (size_t i = 0; i < pointLights.size(); i++)
		{
			memcpy(location, &pointLights[i].asset, sizeof(PointLight));
			location += sizeof(PointLight);

			memcpy(location, &pointLights[i].transform.position, sizeof(glm::vec3));
			location += sizeof(glm::vec3);
		}

		pDevContext->Unmap(pPointLightBuffer, 0);
	}

	void Renderer::expandDirLights()
	{
		static const size_t Increase = 5;
		static const size_t GPUDirLightSize = sizeof(DirectionalLight) + sizeof(glm::vec3);
		HRESULT hr = E_FAIL;

		DX11_RELEASE(pDirLightBuffer);
		DX11_RELEASE(pDirLightSRV);

		dirLights.reserve(dirLights.size() + Increase);

		hr = DX11::createStructuredBuffer(&pDirLightBuffer, dirLights.data(), GPUDirLightSize, (uint32_t)dirLights.capacity(), false);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating dirLight structured buffer");

		hr = DX11::createStructuredSRV(&pDirLightSRV, pDirLightBuffer, (uint32_t)dirLights.capacity());
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating dirLight SRV");

		pDevContext->PSSetShaderResources(4, 1, &pDirLightSRV);
	}

	void Renderer::updateDirLightsBuffer()
	{
		D3D11_MAPPED_SUBRESOURCE sub;
		if (FAILED(pDevContext->Map(pDirLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub)))
			return;

		glm::vec3 dirLightDirection{};
		char* location = (char*)sub.pData;
		for (size_t i = 0; i < dirLights.size(); i++)
		{
			memcpy(location, &dirLights[i].asset, sizeof(DirectionalLight));
			location += sizeof(DirectionalLight);

			dirLightDirection = dirLights[i].transform.forward();
			memcpy(location, &dirLightDirection, sizeof(glm::vec3));
			location += sizeof(glm::vec3);
		}

		pDevContext->Unmap(pDirLightBuffer, 0);
	}

	void Renderer::bindNecessities()
	{
		pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pDevContext->VSSetConstantBuffers(0, 1, &pCameraBuffer);
		pDevContext->VSSetConstantBuffers(1, 1, &pWorldBuffer);
		pDevContext->VSSetConstantBuffers(2, 1, &pShaderDataBuffer);

		pDevContext->RSSetViewports(1u, &viewport);

		pDevContext->PSSetConstantBuffers(0, 1, &pCameraBuffer);
		pDevContext->PSSetConstantBuffers(3, 1, &pMaterialBuffer);
		pDevContext->PSSetShaderResources(3, 1, &pPointLightSRV);
		pDevContext->PSSetConstantBuffers(4, 1, &pLightInfoBuffer);

	}

	void Renderer::bindMeshPipeline()
	{
		pDevContext->IASetInputLayout(pMeshIL);
		pDevContext->VSSetShader(pMeshVS, nullptr, 0);
	}

	void Renderer::bindSkeletalPipeline()
	{
		pDevContext->IASetInputLayout(pAniIL);
		pDevContext->VSSetShader(pAniVS, nullptr, 0);
	}

	void Renderer::createVertexShaders()
	{
		DX11& dx11 = DX11::getInstance();
		std::string shaderData;
		bool result = false;
		HRESULT hr = E_FAIL;

		D3D11_INPUT_ELEMENT_DESC desc[3] = {
			{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"UV",			0, DXGI_FORMAT_R32G32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		result = Shader::readShader("MeshVS", shaderData);
		OKAY_ASSERT(result, "Failed reading MeshVS.cso");

		hr = dx11.getDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &pMeshVS);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed creating static mesh Vertex Shader");

		hr = dx11.getDevice()->CreateInputLayout(desc, 3, shaderData.c_str(), shaderData.length(), &pMeshIL);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed creating static mesh Input Layout");

		return;

		/*D3D11_INPUT_ELEMENT_DESC aniDesc[5] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"JOINTIDX", 0, DXGI_FORMAT_R32G32B32A32_UINT,  1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"WEIGHTS",	 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"UV",		 0, DXGI_FORMAT_R32G32_FLOAT,		2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,	3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		result = Shader::readShader("SkeletalMeshVS.cso", shaderData);
		OKAY_ASSERT(result, "Failed reading SkeletalMeshVS.cso");

		hr = dx11.getDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &pAniVS);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed creating skeletal mesh Vertex Shader");

		hr = dx11.getDevice()->CreateInputLayout(desc, 3, shaderData.c_str(), shaderData.length(), &pAniIL);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed creating skeletal mesh Input Layout");*/
	}

	void Renderer::createPixelShaders()
	{

	}
}
