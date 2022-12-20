#include "Renderer.h"
#include "Engine/Okay/Okay.h"
#include "ContentBrowser.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Okay
{
	Renderer::Renderer(ContentBrowser& content)
		:pMeshIL(), pMeshVS(), pDevContext(DX11::getInstance().getDeviceContext()), 
		defaultPixelShader("PhongPS.cso") ,content(content)
	{
		glm::mat4 Identity4x4(1.f);

		//DX11::createConstantBuffer(&pMaterialBuffer, nullptr, sizeof(Material::GPUData), false);
		DX11::createConstantBuffer(&pViewProjectBuffer, nullptr, sizeof(glm::mat4), false);
		DX11::createConstantBuffer(&pWorldBuffer, &Identity4x4, sizeof(glm::mat4), false);
		//DX11::createConstantBuffer(&pLightInfoBuffer, nullptr, 16, false);

		createVertexShaders();
		createPixelShaders();
		{
			ID3D11SamplerState* simp;
			D3D11_SAMPLER_DESC desc;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.MinLOD = -FLT_MAX;
			desc.MaxLOD = FLT_MAX;
			desc.MipLODBias = 0.f;
			desc.MaxAnisotropy = 1U;
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
		//numLights = 0;



		// Temp ---
		glm::mat4 viewMatrix = glm::lookAtLH(glm::vec3(10.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 projMatrix = glm::perspectiveFovLH(3.14f * 0.5f, 1600.f, 900.f, 0.1f, 1000.f);
		glm::mat4 viewProj = (projMatrix * viewMatrix);
		//glm::mat4 viewProj = glm::transpose(projMatrix * viewMatrix);
		DX11::updateBuffer(pViewProjectBuffer, &viewProj, sizeof(glm::mat4));

		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.Width = 1600.f;
		viewport.Height = 900.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;

		// --- Temp


		bindNecessities();
	}

	Renderer::~Renderer()
	{
		shutdown();
	}

	void Renderer::submit(MeshComponent* pMesh, Transform* pTransform)
	{
		if (numActiveMeshes >= meshesToRender.size())
			meshesToRender.resize(meshesToRender.size() + 10ull);

		meshesToRender[numActiveMeshes].mesh = pMesh;
		meshesToRender[numActiveMeshes].transform = pTransform;

		++numActiveMeshes;
	}

	/*void Renderer::SumbitSkeletal(CompSkeletalMesh* pMesh, CompTransform* pTransform)
	{
		if (numSkeletalActive >= skeletalMeshes.size())
			skeletalMeshes.resize(skeletalMeshes.size() + 10);

		skeletalMeshes[numSkeletalActive].first = pMesh;
		skeletalMeshes[numSkeletalActive].second = pTransform;

		++numSkeletalActive;
	}

	void Renderer::SubmitLight(CompPointLight* pLight, CompTransform* pTransform)
	{
		if (numLights >= lights.size())
			ExpandPointLights();

		lights[numLights].lightData = *pLight;
		lights[numLights].pos = pTransform->position;

		++numLights;
	}*/

	void Renderer::newFrame()
	{
		numActiveMeshes = 0;
		//numSkeletalActive = 0;
		//numLights = 0;
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

	void Renderer::render()
	{
		// Update generic buffers
		//DX11::updateBuffer(pViewProjectBuffer, &mainCamera->GetViewProjectMatrix(), sizeof(DirectX::XMFLOAT4X4));

		/*if (numLights)
		{
			DX11::UpdateBuffer(pPointLightBuffer, lights.data(), UINT(sizeof(GPUPointLight) * numLights));
			DX11::UpdateBuffer(pLightInfoBuffer, &numLights, 4);
		}*/


		// Preperation
		//std::shared_ptr<const Material> material;
		size_t i = 0;

		// Draw static meshes
		bindMeshPipeline();

		// Temp
		pDevContext->OMSetRenderTargets(1u, DX11::getInstance().getBackBufferRTV(), *DX11::getInstance().getDepthBufferDSV());
		defaultPixelShader.bind();

		for (i = 0; i < numActiveMeshes; i++)
		{
			const MeshComponent& cMesh = *meshesToRender.at(i).mesh;
			const Transform& cTransform = *meshesToRender.at(i).transform;

			//material = cMesh.GetMaterial();

			//material->BindTextures();

			DX11::updateBuffer(pWorldBuffer, &cTransform.matrix, sizeof(glm::mat4));
			//DX11::updateBuffer(pMaterialBuffer, &material->GetGPUData(), sizeof(MaterialGPUData));

			Mesh& mesh = content.getMesh(cMesh.meshIdx);
			
			// IA
			pDevContext->IASetVertexBuffers(0u, Mesh::NumBuffers, mesh.getBuffers(), Mesh::Stride, Mesh::Offset);
			pDevContext->IASetIndexBuffer(mesh.getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0u);

			// VS
			
			// RS
			
			// PS
			// Bind material...

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

		lights.resize(numLights + Increase);

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
		pDevContext->PSSetConstantBuffers(3, 1, &pMaterialBuffer);

		pDevContext->PSSetShaderResources(3, 1, &pPointLightSRV);
		pDevContext->PSSetConstantBuffers(4, 1, &pLightInfoBuffer);

		pDevContext->RSSetViewports(1u, &viewport);
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
			{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		result = Shader::readShader("MeshVS.cso", shaderData);
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
			{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,	2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
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
