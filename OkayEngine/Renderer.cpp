#include "Renderer.h"

Renderer::Renderer()
	:pInputLayout(), pVertexShader(), pHullShader(), pDomainShader(), pDevContext(DX11::Get().GetDeviceContext())
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
	
	aniVS;
	CreateVS();
	CreateHS();
	CreateDS();
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

	Bind();
	shaderModel->Bind();
	
	meshesToRender.resize(10);
	numActive = 0;
	numLights = 0;


	CreateSkeletal();
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
	
	meshesToRender.at(numActive).mesh = pMesh;
	meshesToRender.at(numActive).transform = pTransform;

	++numActive;
}

void Renderer::SubmitLight(Okay::CompPointLight* pLight, Okay::CompTransform* pTransform)
{
	if (numLights >= lights.size())
		ExpandPointLights();

	lights.at(numLights).lightData = *pLight;
	lights.at(numLights).pos = pTransform->position;

	++numLights;
}

void Renderer::NewFrame()
{
	numActive = 0;
	numLights = 0;
}

void Renderer::Shutdown()
{
	shaderModel->Shutdown();

	DX11_RELEASE(pViewProjectBuffer);
	DX11_RELEASE(pWorldBuffer);
	DX11_RELEASE(pMaterialBuffer);

	DX11_RELEASE(pInputLayout);
	DX11_RELEASE(pVertexShader);
	DX11_RELEASE(pHullShader);
	DX11_RELEASE(pDomainShader);

	DX11_RELEASE(pPointLightBuffer);
	DX11_RELEASE(pPointLightSRV);
	DX11_RELEASE(pLightInfoBuffer);
}

void Renderer::Render()
{
	using namespace Okay;

	shaderModel->Bind();
	mainCamera->Update();

	DX11::UpdateBuffer(pViewProjectBuffer, &mainCamera->GetViewProjectMatrix(), sizeof(DirectX::XMFLOAT4X4));
	
	if (numLights)
	{
		DX11::UpdateBuffer(pPointLightBuffer, lights.data(), UINT(sizeof(GPUPointLight) * numLights));
		DX11::UpdateBuffer(pLightInfoBuffer, &numLights, 4);
	}

	for (size_t i = 0; i < numActive; i++)
	{
		CompMesh& cMesh = *meshesToRender.at(i).mesh;
		const CompTransform& transform = *meshesToRender.at(i).transform;
		auto material = cMesh.GetMaterial();
		auto mesh = cMesh.GetMesh();

		DX11::UpdateBuffer(pWorldBuffer, &transform.matrix, sizeof(DirectX::XMFLOAT4X4));
		DX11::UpdateBuffer(pMaterialBuffer, &material->GetGPUData(), sizeof(MaterialGPUData));

		mesh->Bind();
		material->BindTextures();
			
		mesh->Draw();
	}


	DX11& dx = DX11::Get();
	pDevContext->IASetInputLayout(aniIL);
	pDevContext->IASetIndexBuffer(goblin->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDevContext->IASetVertexBuffers(0, Okay::SkeletalMesh::NumBuffers, goblin->vertexBuffer, Okay::SkeletalMesh::Stride, Okay::SkeletalMesh::Offset);

	pDevContext->VSSetShader(aniVS, nullptr, 0);

	shaderModel->UnBind();
}

bool Renderer::ExpandPointLights()
{
	static const UINT Increase = 5;
	ID3D11Device* pDevice = DX11::Get().GetDevice();

	DX11_RELEASE(pPointLightBuffer);
	DX11_RELEASE(pPointLightSRV);

	lights.resize(numLights + Increase);

	VERIFY_HR_BOOL(DX11::CreateStructuredBuffer<GPUPointLight>(&pPointLightBuffer, lights.data(), (UINT)lights.size(), false));
	VERIFY_HR_BOOL(DX11::CreateStructuredSRV<GPUPointLight>(&pPointLightSRV, pPointLightBuffer, (UINT)lights.size()));

	pDevContext->PSSetShaderResources(3, 1, &pPointLightSRV);

	return true;
}

void Renderer::Bind()
{
	pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDevContext->IASetInputLayout(pInputLayout);

	pDevContext->VSSetShader(pVertexShader, nullptr, 0);
	pDevContext->VSSetConstantBuffers(0, 1, &pViewProjectBuffer);
	pDevContext->VSSetConstantBuffers(1, 1, &pWorldBuffer);
	pDevContext->PSSetConstantBuffers(3, 1, &pMaterialBuffer);

	pDevContext->PSSetShaderResources(3, 1, &pPointLightSRV);
	pDevContext->PSSetConstantBuffers(4, 1, &pLightInfoBuffer);

	

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
	VERIFY_HR_BOOL(DX11::Get().GetDevice()->CreateInputLayout(desc, 3, shaderData.c_str(), shaderData.length(), &pInputLayout));
	VERIFY_HR_BOOL(DX11::Get().GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &pVertexShader));


	D3D11_INPUT_ELEMENT_DESC aniDesc[5] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV",		 0, DXGI_FORMAT_R32G32_FLOAT,		1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"JOINTIDX", 0, DXGI_FORMAT_R32G32B32A32_UINT,  2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WEIGHTS",	 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	VERIFY(Okay::ReadShader("SkeletalMeshVS.cso", shaderData));
	VERIFY_HR_BOOL(DX11::Get().GetDevice()->CreateInputLayout(aniDesc, 5, shaderData.c_str(), shaderData.length(), &aniIL));
	VERIFY_HR_BOOL(DX11::Get().GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &aniVS));


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
