#include "Renderer.h"
#include "Engine.h"

Renderer::Renderer()
	:pMeshIL(), pMeshVS(), pHullShader(), pDomainShader(), pDevContext(DX11::Get().GetDeviceContext())
{
	// Make sure Okay::Engine::Get() is never called here

	shaderModel = std::make_unique<Okay::ShaderModel>(true);
	mainCamera = std::make_unique<Okay::Camera>();

	DirectX::XMFLOAT4X4 Identity4x4;
	DirectX::XMStoreFloat4x4(&Identity4x4, DirectX::XMMatrixIdentity());

	DX11::CreateConstantBuffer(&pMaterialBuffer, nullptr, sizeof(Okay::MaterialGPUData), false);
	DX11::CreateConstantBuffer(&pViewProjectBuffer, &mainCamera->GetViewProjectMatrix(), sizeof(DirectX::XMFLOAT4X4), false);
	DX11::CreateConstantBuffer(&pWorldBuffer, &Identity4x4, sizeof(DirectX::XMFLOAT4X4), false);
	DX11::CreateConstantBuffer(&pLightInfoBuffer, nullptr, 16, false);

	CreateVS();
	CreateHS();
	CreateDS();
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
		DX11::Get().GetDevice()->CreateSamplerState(&desc, &simp);

		pDevContext->PSSetSamplers(0, 1, &simp);
		simp->Release();
	}

	BindBasics();
	shaderModel->Bind();

	meshesToRender.resize(10);
	numActive = 0;

	skeletalMeshes.resize(10);
	numSkeletalActive = 0;

	numLights = 0;
}

Renderer::~Renderer()
{
	Shutdown();
}

void Renderer::Resize()
{
	shaderModel->Resize();
}

void Renderer::Submit(Okay::CompMesh* pMesh, Okay::CompTransform* pTransform)
{
	if (numActive >= meshesToRender.size())
		meshesToRender.resize(meshesToRender.size() + 10);

	meshesToRender[numActive].mesh = pMesh;
	meshesToRender[numActive].transform = pTransform;

	++numActive;
}

void Renderer::SumbitSkeletal(Okay::CompSkeletalMesh* pMesh, Okay::CompTransform* pTransform)
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
}

void Renderer::NewFrame()
{
	numActive = 0;
	numSkeletalActive = 0;
	numLights = 0;
}

void Renderer::Shutdown()
{
	shaderModel->Shutdown();

	DX11_RELEASE(pViewProjectBuffer);
	DX11_RELEASE(pWorldBuffer);
	DX11_RELEASE(pMaterialBuffer);

	DX11_RELEASE(pMeshIL);
	DX11_RELEASE(pMeshVS);
	DX11_RELEASE(pHullShader);
	DX11_RELEASE(pDomainShader);

	DX11_RELEASE(pPointLightBuffer);
	DX11_RELEASE(pPointLightSRV);
	DX11_RELEASE(pLightInfoBuffer);

	DX11_RELEASE(pAniVS);
	DX11_RELEASE(pAniIL);
}

void Renderer::Render()
{
	using namespace Okay;

	shaderModel->Bind();
	mainCamera->Update();

	// Update generic buffers
	DX11::UpdateBuffer(pViewProjectBuffer, &mainCamera->GetViewProjectMatrix(), sizeof(DirectX::XMFLOAT4X4));

	if (numLights)
	{
		DX11::UpdateBuffer(pPointLightBuffer, lights.data(), UINT(sizeof(GPUPointLight) * numLights));
		DX11::UpdateBuffer(pLightInfoBuffer, &numLights, 4);
	}


	// Preperation
	std::shared_ptr<const Material> material;
	size_t i = 0;

	// Draw static meshes
	BindMeshPipeline();

	for (i = 0; i < numActive; i++)
	{
		const CompMesh& cMesh = *meshesToRender.at(i).mesh;
		const CompTransform& transform = *meshesToRender.at(i).transform;

		material = cMesh.GetMaterial();

		material->BindTextures();

		DX11::UpdateBuffer(pWorldBuffer, &transform.matrix, sizeof(DirectX::XMFLOAT4X4));
		DX11::UpdateBuffer(pMaterialBuffer, &material->GetGPUData(), sizeof(MaterialGPUData));

		cMesh.GetMesh()->Draw();
	}


	// Draw skeletal meshes
	BindSkeletalPipeline();

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
		
	}

}

bool Renderer::ExpandPointLights()
{
	static const UINT Increase = 5;

	DX11_RELEASE(pPointLightBuffer);
	DX11_RELEASE(pPointLightSRV);

	lights.resize(numLights + Increase);

	VERIFY_HR_BOOL(DX11::CreateStructuredBuffer(&pPointLightBuffer, lights.data(), sizeof(GPUPointLight), (UINT)lights.size(), false));
	VERIFY_HR_BOOL(DX11::CreateStructuredSRV(&pPointLightSRV, pPointLightBuffer, (UINT)lights.size()));

	pDevContext->PSSetShaderResources(3, 1, &pPointLightSRV);

	return true;
}

void Renderer::BindBasics()
{
	pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pDevContext->VSSetConstantBuffers(0, 1, &pViewProjectBuffer);
	pDevContext->VSSetConstantBuffers(1, 1, &pWorldBuffer);
	pDevContext->PSSetConstantBuffers(3, 1, &pMaterialBuffer);

	pDevContext->PSSetShaderResources(3, 1, &pPointLightSRV);
	pDevContext->PSSetConstantBuffers(4, 1, &pLightInfoBuffer);
}

void Renderer::BindMeshPipeline()
{
	pDevContext->IASetInputLayout(pMeshIL);
	pDevContext->VSSetShader(pMeshVS, nullptr, 0);
}

void Renderer::BindSkeletalPipeline()
{
	pDevContext->IASetInputLayout(pAniIL);
	pDevContext->VSSetShader(pAniVS, nullptr, 0);
}

bool Renderer::CreateVS()
{
	std::string shaderData;

	D3D11_INPUT_ELEMENT_DESC desc[3] = {
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV",			0, DXGI_FORMAT_R32G32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	VERIFY(Okay::ReadShader("MeshVS.cso", shaderData));
	VERIFY_HR_BOOL(DX11::Get().GetDevice()->CreateInputLayout(desc, 3, shaderData.c_str(), shaderData.length(), &pMeshIL));
	VERIFY_HR_BOOL(DX11::Get().GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &pMeshVS));

	D3D11_INPUT_ELEMENT_DESC aniDesc[5] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"JOINTIDX", 0, DXGI_FORMAT_R32G32B32A32_UINT,  1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WEIGHTS",	 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV",		 0, DXGI_FORMAT_R32G32_FLOAT,		2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,	2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	VERIFY(Okay::ReadShader("SkeletalMeshVS.cso", shaderData));
	VERIFY_HR_BOOL(DX11::Get().GetDevice()->CreateInputLayout(aniDesc, 5, shaderData.c_str(), shaderData.length(), &pAniIL));
	VERIFY_HR_BOOL(DX11::Get().GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &pAniVS));

	return true;
}

bool Renderer::CreateHS()
{
	return false; // Disabled
}

bool Renderer::CreateDS()
{
	return false; // Disabled
}