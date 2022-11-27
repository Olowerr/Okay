#include "Renderer.h"
#include "Engine/Okay/Okay.h"

Renderer::Renderer()
	:pMeshIL(), pMeshVS(), pDevContext(DX11::getInstance().getDeviceContext()), defaultPixelShader("PhongPS.cso")
{
	// Make sure Okay::Engine::Get() is never called here
	
	Okay::Float4x4 Identity4x4(1.f);

	//DX11::createConstantBuffer(&pMaterialBuffer, nullptr, sizeof(Okay::Material::GPUData), false);
	//DX11::createConstantBuffer(&pViewProjectBuffer, &mainCamera->GetViewProjectMatrix(), sizeof(DirectX::XMFLOAT4X4), false);
	DX11::createConstantBuffer(&pWorldBuffer, &Identity4x4, sizeof(Okay::Float4x4), false);
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

	bindNecessities();

	meshesToRender.resize(10);
	numActiveMeshes = 0;

	//skeletalMeshes.resize(10);
	//numSkeletalActive = 0;
	//
	//numLights = 0;
}

Renderer::~Renderer()
{
	shutdown();
}

void Renderer::submit(Okay::MeshComponent* pMesh, Okay::Transform* pTransform)
{
	if (numActiveMeshes >= meshesToRender.size())
		meshesToRender.resize(meshesToRender.size() + 10ull);

	meshesToRender[numActiveMeshes].mesh = pMesh;
	meshesToRender[numActiveMeshes].transform = pTransform;

	++numActiveMeshes;
}

/*void Renderer::SumbitSkeletal(Okay::CompSkeletalMesh* pMesh, Okay::CompTransform* pTransform)
{
	if (numSkeletalActive >= skeletalMeshes.size())
		skeletalMeshes.resize(skeletalMeshes.size() + 10);

	skeletalMeshes[numSkeletalActive].first = pMesh;
	skeletalMeshes[numSkeletalActive].second = pTransform;

	++numSkeletalActive;
}

void Renderer::SubmitLight(Okay::CompPointLight* pLight, Okay::CompTransform* pTransform)
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
	using namespace Okay;

	//shaderModel->Bind();
	//mainCamera->Update();

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

	for (i = 0; i < numActiveMeshes; i++)
	{
		const MeshComponent& cMesh = *meshesToRender.at(i).mesh;
		const Transform& cTransform = *meshesToRender.at(i).transform;

		//material = cMesh.GetMaterial();

		//material->BindTextures();

		DX11::updateBuffer(pWorldBuffer, &cTransform.matrix, sizeof(Okay::Float4x4));
		//DX11::updateBuffer(pMaterialBuffer, &material->GetGPUData(), sizeof(MaterialGPUData));


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

	hr = DX11::createStructuredBuffer(&pPointLightBuffer, lights.data(), sizeof(GPUPointLight), (uint32)lights.size(), false);
	OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating pointLight structured buffer");

	hr = DX11::createStructuredSRV(&pPointLightSRV, pPointLightBuffer, (uint32)lights.size());
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

	result = Okay::Shader::readShader("MeshVS.cso", shaderData);
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

	result = Okay::Shader::readShader("SkeletalMeshVS.cso", shaderData);
	OKAY_ASSERT(result, "Failed reading SkeletalMeshVS.cso");

	hr = dx11.getDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &pAniVS);
	OKAY_ASSERT(SUCCEEDED(hr), "Failed creating skeletal mesh Vertex Shader");

	hr = dx11.getDevice()->CreateInputLayout(desc, 3, shaderData.c_str(), shaderData.length(), &pAniIL);
	OKAY_ASSERT(SUCCEEDED(hr), "Failed creating skeletal mesh Input Layout");
}

void Renderer::createPixelShaders()
{

}