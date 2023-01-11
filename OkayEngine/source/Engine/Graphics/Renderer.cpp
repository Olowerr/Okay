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

namespace Okay
{
	Renderer::Renderer(const RenderTexture* pRenderTarget, const ContentBrowser& content)
		:pMeshIL(), pMeshVS(), pDevContext(DX11::getInstance().getDeviceContext()), 
		defaultPixelShader("PhongPS.cso") ,content(content), pRenderTarget(pRenderTarget)
	{
		OKAY_ASSERT(pRenderTarget, "RenderTarget was nullptr");

		glm::mat4 Identity4x4(1.f);

		DX11::createConstantBuffer(&pMaterialBuffer, nullptr, sizeof(Material::GPUData), false);
		DX11::createConstantBuffer(&pViewProjectBuffer, nullptr, sizeof(glm::mat4), false);
		DX11::createConstantBuffer(&pWorldBuffer, &Identity4x4, sizeof(glm::mat4), false);
		DX11::createConstantBuffer(&pLightInfoBuffer, nullptr, 16, false);

		createVertexShaders();
		createPixelShaders();
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
			simp->Release();
		}


		meshesToRender.resize(10);
		numActiveMeshes = 0;

		//skeletalMeshes.resize(10);
		//numSkeletalActive = 0;
		//
		//numPointLights = 0;

		const glm::ivec2 dims = pRenderTarget->getDimensions();
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.Width = (float)dims.x;
		viewport.Height = (float)dims.y; 
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;


		bindNecessities();
	}

	Renderer::~Renderer()
	{
		shutdown();
	}

	void Renderer::submit(const MeshComponent* pMesh, const Transform* pTransform)
	{
		if (numActiveMeshes >= meshesToRender.size())
			meshesToRender.resize(meshesToRender.size() + 10ull);

		meshesToRender[numActiveMeshes].pMesh = pMesh;
		meshesToRender[numActiveMeshes].pTransform = pTransform;

		++numActiveMeshes;
	}

	/*void Renderer::SumbitSkeletal(CompSkeletalMesh* pMesh, CompTransform* pTransform)
	{
		if (numSkeletalActive >= skeletalMeshes.size())
			skeletalMeshes.resize(skeletalMeshes.size() + 10);

		skeletalMeshes[numSkeletalActive].first = pMesh;
		skeletalMeshes[numSkeletalActive].second = pTransform;

		++numSkeletalActive;
	}*/

	void Renderer::submitPointLight(const PointLight& pLight, const Transform& pTransform)
	{
		if (numPointLights >= lights.size())
			expandPointLights();

		lights[numPointLights].lightData = pLight;
		lights[numPointLights].pos = pTransform.position;

		++numPointLights;
	}

	void Renderer::setRenderTexture(const RenderTexture* pRenderTexture)
	{
		OKAY_ASSERT(pRenderTexture, "RenderTarget was nullptr");
		pRenderTarget = pRenderTexture;

		const glm::ivec2 dims = pRenderTarget->getDimensions();
		viewport.Width = (float)dims.x;
		viewport.Height = (float)dims.y;

		pDevContext->RSSetViewports(1u, &viewport);
	}

	void Renderer::newFrame()
	{
		numActiveMeshes = 0;
		//numSkeletalActive = 0;
		numPointLights = 0;
	}

	void Renderer::shutdown()
	{
		DX11_RELEASE(pViewProjectBuffer);
		DX11_RELEASE(pWorldBuffer);
		DX11_RELEASE(pMaterialBuffer);

		DX11_RELEASE(pMeshIL);
		DX11_RELEASE(pMeshVS);

		DX11_RELEASE(pPointLightBuffer);
		DX11_RELEASE(pPointLightSRV);
		DX11_RELEASE(pLightInfoBuffer);

		DX11_RELEASE(pAniVS);
		DX11_RELEASE(pAniIL);
	}

	void Renderer::render(const Entity& cameraEntity)
	{
		
		DX11::updateBuffer(pPointLightBuffer, lights.data(), uint32_t(sizeof(GPUPointLight) * numPointLights));
		DX11::updateBuffer(pLightInfoBuffer, &numPointLights, 4);
		
		
		// Calculate viewProjection matrix
		OKAY_ASSERT(cameraEntity.hasComponent<Camera>(), "MainCamera doesn't have a Camera Component");
		const Camera& camera = cameraEntity.getComponent<Camera>();
		const Transform& camTransform = cameraEntity.getComponent<Okay::Transform>();
		glm::mat4 viewProjMatrix =  glm::transpose(camera.projectionMatrix *
			glm::lookAtLH(camTransform.position, camTransform.position + camTransform.forward(), camTransform.up()));
		
		DX11::updateBuffer(pViewProjectBuffer, &viewProjMatrix, sizeof(glm::mat4));

		// Bind static mesh pipeline
		bindMeshPipeline();

		pDevContext->OMSetRenderTargets(1u, pRenderTarget->getRTV(), *pRenderTarget->getDSV());
		defaultPixelShader.bind();
		
		// Preperation
		ID3D11ShaderResourceView* textures[3] = {};
		size_t i = 0;
		glm::mat4 worldMatrix{};

		// Draw all statis meshes
		for (i = 0; i < numActiveMeshes; i++)
		{
			const MeshComponent& cMesh = *meshesToRender.at(i).pMesh;
			const Mesh& mesh = content.getMesh(cMesh.meshIdx);
			Transform& cTransform = const_cast<Transform&>(*meshesToRender.at(i).pTransform);

			// Temp - make more robust system (Update only if entity has script?) // No.. can miss entities
			cTransform.calculateMatrix();
			worldMatrix = glm::transpose(cTransform.matrix);

			const Material& material = content.getMaterial(cMesh.materialIdx);
			textures[Material::BASECOLOUR_INDEX] = content.getTexture(material.getBaseColour()).getSRV();
			textures[Material::SPECULAR_INDEX]	 = content.getTexture(material.getSpecular()).getSRV();
			textures[Material::AMBIENT_INDEX]	 = content.getTexture(material.getAmbient()).getSRV();

			DX11::updateBuffer(pWorldBuffer, &worldMatrix, sizeof(glm::mat4));
			DX11::updateBuffer(pMaterialBuffer, &material.getGPUData(), sizeof(Material::GPUData));

			
			// IA
			pDevContext->IASetVertexBuffers(0u, Mesh::NumBuffers, mesh.getBuffers(), Mesh::Stride, Mesh::Offset);
			pDevContext->IASetIndexBuffer(mesh.getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0u);

			// VS
			
			// RS
			
			// PS
			pDevContext->PSSetShaderResources(0u, 3u, textures);

			// OM

			// Draw
			pDevContext->DrawIndexed(mesh.getNumIndices(), 0u, 0u);

		}


		// Draw skeletal meshes
		/*BindSkeletalPipeline();

		for (i = 0; i < numSkeletalActive; i++)
		{
			CompSkeletalMesh& cMesh = *skeletalMeshes[i].first;
			const CompTransform& transform = *skeletalMeshes[i].second;

			material = cMesh.GetMaterial();
			material->BindTextures();
			cMesh.UpdateSkeletalMatrices();

			DX11::UpdateBuffer(pWorldBuffer, &transform.matrix, sizeof(DirectX::XMFLOAT4X4));
			DX11::UpdateBuffer(pMaterialBuffer, &material->GetGPUData(), sizeof(MaterialGPUData));

			cMesh.GetMesh()->Draw();

		}*/
	}

	void Renderer::expandPointLights()
	{
		static const UINT Increase = 5;
		HRESULT hr = E_FAIL;

		DX11_RELEASE(pPointLightBuffer);
		DX11_RELEASE(pPointLightSRV);

		lights.resize(numPointLights + Increase);

		hr = DX11::createStructuredBuffer(&pPointLightBuffer, lights.data(), sizeof(GPUPointLight), (uint32_t)lights.size(), false);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating pointLight structured buffer");

		hr = DX11::createStructuredSRV(&pPointLightSRV, pPointLightBuffer, (uint32_t)lights.size());
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating pointLight SRV");

		pDevContext->PSSetShaderResources(3, 1, &pPointLightSRV);
	}

	void Renderer::bindNecessities()
	{
		pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pDevContext->VSSetConstantBuffers(0, 1, &pViewProjectBuffer);
		pDevContext->VSSetConstantBuffers(1, 1, &pWorldBuffer);

		pDevContext->RSSetViewports(1u, &viewport);

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
	}

	void Renderer::createPixelShaders()
	{

	}
}
