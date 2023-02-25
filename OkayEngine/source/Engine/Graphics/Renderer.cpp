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
	std::unique_ptr<Renderer::PipelineResources> Renderer::pipeline;

	void Renderer::init()
	{
		bool result = false;
		HRESULT hr = E_FAIL;

		pipeline = std::make_unique<PipelineResources>();
		DX11& dx11 = DX11::get();

		// Buffers
		{ 
			hr = DX11::createConstantBuffer(&pipeline->pMaterialBuffer, nullptr, sizeof(Material::GPUData), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating materialBuffer");

			hr = DX11::createConstantBuffer(&pipeline->pCameraBuffer, nullptr, sizeof(GPUCamera), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating cameraBuffer");

			hr = DX11::createConstantBuffer(&pipeline->pWorldBuffer, nullptr, sizeof(glm::mat4), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating worldBuffer");

			hr = DX11::createConstantBuffer(&pipeline->pLightInfoBuffer, nullptr, sizeof(LightInfo), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating lightInfoBuffer");

			hr = DX11::createConstantBuffer(&pipeline->pShaderDataBuffer, nullptr, sizeof(Shader::GPUData), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating shaderDataBuffer");

			expandPointLights();
			expandDirLights();
		}

		// Wireframe RS
		{
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
			hr = dx11.getDevice()->CreateRasterizerState(&rsDesc, &pipeline->pWireframeRS);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating wireframeRS");
		}
		

		// Basic linear sampler
		{
			ID3D11SamplerState* simp;
			D3D11_SAMPLER_DESC simpDesc{};
			simpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			simpDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			simpDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			simpDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			simpDesc.MinLOD = -FLT_MAX;
			simpDesc.MaxLOD = FLT_MAX;
			simpDesc.MipLODBias = 0.f;
			simpDesc.MaxAnisotropy = 1u;
			simpDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			hr = dx11.getDevice()->CreateSamplerState(&simpDesc, &simp);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating sampler");

			dx11.getDeviceContext()->PSSetSamplers(0, 1, &simp);
			dx11.getDeviceContext()->VSSetSamplers(0, 1, &simp);
			simp->Release();
		}
		

		// Input Layouts & Shaders
		{
			std::string shaderData;	

			D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[3] = {
				{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"UV",			0, DXGI_FORMAT_R32G32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			result = readBinary(SHADER_PATH "MeshVS.cso", shaderData);
			OKAY_ASSERT(result, "Failed reading MeshVS.cso");

			hr = dx11.getDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &pipeline->pMeshVS);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating static mesh Vertex Shader");

			hr = dx11.getDevice()->CreateInputLayout(inputLayoutDesc, 3, shaderData.c_str(), shaderData.length(), &pipeline->pMeshIL);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating static mesh Input Layout");

#if 0 // Skeletal Animation (OLD)
			D3D11_INPUT_ELEMENT_DESC aniDesc[5] = {
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
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating skeletal mesh Input Layout");
#endif
		}

		// Bind necessities
		{
			ID3D11DeviceContext* pDevContext = dx11.getDeviceContext();
			pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			pDevContext->VSSetConstantBuffers(0, 1, &pipeline->pCameraBuffer);
			pDevContext->VSSetConstantBuffers(1, 1, &pipeline->pWorldBuffer);
			pDevContext->VSSetConstantBuffers(2, 1, &pipeline->pShaderDataBuffer);

			pDevContext->PSSetConstantBuffers(0, 1, &pipeline->pCameraBuffer);
			pDevContext->PSSetConstantBuffers(3, 1, &pipeline->pMaterialBuffer);
			pDevContext->PSSetShaderResources(3, 1, &pipeline->pPointLightSRV);
			pDevContext->PSSetConstantBuffers(4, 1, &pipeline->pLightInfoBuffer);
		}
		
	}

	Renderer::Renderer(RenderTexture* pRenderTarget)
		:pDevContext(DX11::get().getDeviceContext()), pRenderTarget(pRenderTarget)
	{
		OKAY_ASSERT(pRenderTarget, "RenderTarget was nullptr");
		
		meshes.reserve(10);
		pointLights.reserve(10);
		dirLights.reserve(2);

		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		glm::ivec2 dims = pRenderTarget->getDimensions();
		onTargetResize((uint32_t)dims.x, (uint32_t)dims.y);
	}

	Renderer::~Renderer()
	{
		DX11_RELEASE(pWireframeRS);
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
		if (pointLights.size() >= (uint32_t)pipeline->maxPointLights)
			expandPointLights();

		pointLights.emplace_back(light, transform);
		lightInfo.numPointLights++;
	}

	void Renderer::submit(const DirectionalLight& light, const Transform& transform)
	{
		if (dirLights.size() >= (uint32_t)pipeline->maxDirLights)
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

		glm::ivec2 dims = pRenderTarget->getDimensions();
		onTargetResize((uint32_t)dims.x, (uint32_t)dims.y);
	}

	void Renderer::onTargetResize(uint32_t width, uint32_t height)
	{
		viewport.Width = (float)width;
		viewport.Height = (float)height;
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

	void Renderer::render()
	{
		updatePointLightsBuffer();
		updateDirLightsBuffer();
		DX11::updateBuffer(pipeline->pLightInfoBuffer, &lightInfo, (uint32_t)sizeof(LightInfo));
		
		calculateCameraMatrix();
		bindMeshPipeline();
		pDevContext->OMSetRenderTargets(1u, pRenderTarget->getRTV(), *pRenderTarget->getDSV());
		pDevContext->RSSetState(pWireframeRS);
		
		ID3D11ShaderResourceView* textures[3] = {};
		glm::mat4 worldMatrix{};

		ContentBrowser& content = ContentBrowser::get();

		// Draw all static meshes
		for (size_t i = 0; i < meshes.size(); i++)
		{
			const MeshComponent& cMesh	= meshes[i].asset;
			const Mesh& mesh			= content.getMesh(cMesh.meshIdx);
			const Material& material	= content.getMaterial(cMesh.materialIdx);
			const Shader& shader		= content.getShader(cMesh.shaderIdx);

			worldMatrix = glm::transpose(meshes[i].transform);

			textures[Material::BASECOLOUR_INDEX] = content.getTexture(material.getBaseColour()).getSRV();
			textures[Material::SPECULAR_INDEX]	 = content.getTexture(material.getSpecular()).getSRV();
			textures[Material::AMBIENT_INDEX]	 = content.getTexture(material.getAmbient()).getSRV();

			DX11::updateBuffer(pipeline->pWorldBuffer, &worldMatrix, sizeof(glm::mat4));
			DX11::updateBuffer(pipeline->pMaterialBuffer, &material.getGPUData(), sizeof(Material::GPUData));
			DX11::updateBuffer(pipeline->pShaderDataBuffer, &shader.getGPUData(), sizeof(Shader::GPUData));


			// IA
			pDevContext->IASetVertexBuffers(0u, Mesh::NumBuffers, mesh.getBuffers(), Mesh::Stride, Mesh::Offset);
			pDevContext->IASetIndexBuffer(mesh.getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0u);

			// PS
			shader.bind();
			pDevContext->PSSetShaderResources(0u, 3u, textures);

			// Draw
			pDevContext->DrawIndexed(mesh.getNumIndices(), 0u, 0u);

		}
	}

	void Renderer::setWireframe(bool wireFrame)
	{
		if (wireFrame)
		{
			pWireframeRS = pipeline->pWireframeRS;
			pWireframeRS->AddRef();
		}
		else
		{
			DX11_RELEASE(pWireframeRS);
		}
	}

	void Renderer::expandPointLights()
	{
		static const size_t GPUPointLightSize = sizeof(PointLight) + sizeof(glm::vec3);
		HRESULT hr = E_FAIL;

		DX11_RELEASE(pipeline->pPointLightBuffer);
		DX11_RELEASE(pipeline->pPointLightSRV);

		pipeline->maxPointLights += 5u;

		hr = DX11::createStructuredBuffer(&pipeline->pPointLightBuffer, nullptr, GPUPointLightSize, pipeline->maxPointLights, false);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating pointLight structured buffer");

		hr = DX11::createStructuredSRV(&pipeline->pPointLightSRV, pipeline->pPointLightBuffer, pipeline->maxPointLights);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating pointLight SRV");

		DX11::get().getDeviceContext()->PSSetShaderResources(3, 1, &pipeline->pPointLightSRV);
	}

	void Renderer::updatePointLightsBuffer()
	{
		D3D11_MAPPED_SUBRESOURCE sub;
		if (FAILED(pDevContext->Map(pipeline->pPointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub)))
			return;

		char* location = (char*)sub.pData;
		for (size_t i = 0; i < pointLights.size(); i++)
		{
			memcpy(location, &pointLights[i].asset, sizeof(PointLight));
			location += sizeof(PointLight);

			memcpy(location, &pointLights[i].transform.position, sizeof(glm::vec3));
			location += sizeof(glm::vec3);
		}

		pDevContext->Unmap(pipeline->pPointLightBuffer, 0);
	}

	void Renderer::expandDirLights()
	{;
		static const size_t GPUDirLightSize = sizeof(DirectionalLight) + sizeof(glm::vec3);
		HRESULT hr = E_FAIL;

		DX11_RELEASE(pipeline->pDirLightBuffer);
		DX11_RELEASE(pipeline->pDirLightSRV);

		pipeline->maxDirLights += 5u;

		hr = DX11::createStructuredBuffer(&pipeline->pDirLightBuffer, nullptr, GPUDirLightSize, pipeline->maxDirLights, false);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating dirLight structured buffer");

		hr = DX11::createStructuredSRV(&pipeline->pDirLightSRV, pipeline->pDirLightBuffer, pipeline->maxDirLights);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating dirLight SRV");

		DX11::get().getDeviceContext()->PSSetShaderResources(4, 1, &pipeline->pDirLightSRV);
	}

	void Renderer::updateDirLightsBuffer()
	{
		D3D11_MAPPED_SUBRESOURCE sub;
		if (FAILED(pDevContext->Map(pipeline->pDirLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub)))
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

		pDevContext->Unmap(pipeline->pDirLightBuffer, 0);
	}

	void Renderer::calculateCameraMatrix()
	{
		const Camera& camera = cameraEntity.getComponent<Camera>();
		Transform& camTransform = cameraEntity.getComponent<Okay::Transform>();

		camTransform.calculateMatrix();
		camData.direction = camTransform.forward();
		camData.pos = camTransform.position;
		camData.viewProjMatrix = glm::transpose(camera.projectionMatrix *
			glm::lookAtLH(camTransform.position, camTransform.position + camData.direction, camTransform.up()));

		DX11::updateBuffer(pipeline->pCameraBuffer, &camData, sizeof(GPUCamera));
	}

	void Renderer::bindMeshPipeline()
	{
		DX11::get().getDeviceContext()->IASetInputLayout(pipeline->pMeshIL);
		DX11::get().getDeviceContext()->VSSetShader(pipeline->pMeshVS, nullptr, 0);
	}

	void Renderer::bindSkeletalPipeline()
	{
		return;
		DX11::get().getDeviceContext()->IASetInputLayout(pipeline->pSkeletalIL);
		DX11::get().getDeviceContext()->VSSetShader(pipeline->pSkeletalVS, nullptr, 0);
	}
	
	Renderer::PipelineResources::~PipelineResources()
	{
		DX11_RELEASE(pCameraBuffer);
		DX11_RELEASE(pWorldBuffer);
		DX11_RELEASE(pMaterialBuffer);
		DX11_RELEASE(pShaderDataBuffer);

		DX11_RELEASE(pMeshIL);
		DX11_RELEASE(pMeshVS);
		DX11_RELEASE(pWireframeRS);

		DX11_RELEASE(pLightInfoBuffer);
		DX11_RELEASE(pPointLightBuffer);
		DX11_RELEASE(pPointLightSRV);
		DX11_RELEASE(pDirLightBuffer);
		DX11_RELEASE(pDirLightSRV);

		DX11_RELEASE(pSkeletalIL);
		DX11_RELEASE(pSkeletalVS);
	}
}
