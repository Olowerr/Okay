#include "Renderer.h"

Renderer::Renderer()
	:pInputLayout(), pVertexShader(), pHullShader(), pDomainShader(), pDevContext(DX11::Get().GetDeviceContext())
{
	// Make sure Okay::Engine::Get() is never called here

	shaderModel = std::make_unique<Okay::ShaderModel>(true);
	mainCamera = std::make_unique<Okay::Camera>();

	DirectX::XMFLOAT4X4 Identity4x4;
	DirectX::XMStoreFloat4x4(&Identity4x4, DirectX::XMMatrixIdentity());

	DX11::CreateConstantBuffer(&pMaterialBuffer, nullptr, sizeof(Okay::Material::GPUData), false);
	DX11::CreateConstantBuffer(&pViewProjectBuffer, &mainCamera->GetViewProjectMatrix(), sizeof(DirectX::XMFLOAT4X4), false);
	DX11::CreateConstantBuffer(&pWorldBuffer, &Identity4x4, sizeof(DirectX::XMFLOAT4X4), false);
	CreateVS();
	CreateHS();
	CreateDS();

	Bind();
	shaderModel->Bind();
	
	meshesToRender.resize(10);
	numActive = 0;
}

Renderer::~Renderer()
{
	Shutdown();
}

void Renderer::Submit(Okay::CompMesh* pMesh, Okay::CompTransform* pTransform)
{
	if (numActive >= meshesToRender.size())
	{
		// Maybe increase more than 50
		meshesToRender.resize(meshesToRender.size() + 50);
	}

	meshesToRender.at(numActive).mesh = pMesh;
	meshesToRender.at(numActive).transform = pTransform;

	++numActive;
}

void Renderer::NewFrame()
{
	numActive = 0;
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
}

void Renderer::Render()
{
	mainCamera->Update();
	DX11::UpdateBuffer(pViewProjectBuffer, &mainCamera->GetViewProjectMatrix(), sizeof(DirectX::XMFLOAT4X4));

	for (size_t i = 0; i < numActive; i++)
	{
		const Okay::CompMesh& mesh = *meshesToRender.at(i).mesh;
		const Okay::CompTransform& transform = *meshesToRender.at(i).transform;

		DX11::UpdateBuffer(pWorldBuffer, &transform.matrix, sizeof(DirectX::XMFLOAT4X4));
		DX11::UpdateBuffer(pMaterialBuffer, &mesh.materials[0]->GetGPUData(), sizeof(Okay::Material::GPUData));

		mesh.mesh->Bind();
		mesh.materials[0]->BindTextures();
			
		mesh.mesh->Draw();
	}
}

void Renderer::Bind()
{
	pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDevContext->IASetInputLayout(pInputLayout);

	pDevContext->VSSetShader(pVertexShader, nullptr, 0);
	pDevContext->VSSetConstantBuffers(0, 1, &pViewProjectBuffer);
	pDevContext->VSSetConstantBuffers(1, 1, &pWorldBuffer);
	pDevContext->PSSetConstantBuffers(3, 1, &pMaterialBuffer);

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

bool Renderer::CreateVS()
{
	std::string shaderData;

	D3D11_INPUT_ELEMENT_DESC desc[3] = {
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV",			0, DXGI_FORMAT_R32G32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	VERIFY(Okay::ReadShader("MeshVS.cso", shaderData));
	VERIFY_HR(DX11::Get().GetDevice()->CreateInputLayout(desc, 3, shaderData.c_str(), shaderData.length(), &pInputLayout));
	VERIFY_HR(DX11::Get().GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &pVertexShader));

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
