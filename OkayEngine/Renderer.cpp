#include "Renderer.h"

Renderer::Renderer()
	:pInputLayout(), pVertexShader(), pHullShader(), pDomainShader(), pDevContext(DX11::Get().GetDeviceContext())
{
	shaderModel = std::make_unique<Okay::ShaderModel>(true);
	mainCamera = std::make_unique<Okay::Camera>();

	DirectX::XMFLOAT4X4 Identity4x4;
	DirectX::XMStoreFloat4x4(&Identity4x4, DirectX::XMMatrixIdentity());

	DX11::CreateConstantBuffer(&pViewProjectBuffer, &mainCamera->GetViewProjectMatrix(), sizeof(DirectX::XMFLOAT4X4), false);
	DX11::CreateConstantBuffer(&pWorldBuffer, &Identity4x4, sizeof(DirectX::XMFLOAT4X4), false);
	CreateVS();
	CreateHS();
	CreateDS();

	Bind();
	shaderModel->Bind();
	
	meshesToRender.resize(10);
	transforms.resize(10);
	numActive = 0;
}

Renderer::~Renderer()
{
	Shutdown();
}

void Renderer::Submit(Entity entity)
{
	if (numActive >= meshesToRender.size())
	{
		// Maybe increase more than 50
		meshesToRender.resize(meshesToRender.size() + 50);
		transforms.resize(meshesToRender.size() + 50);
	}

	meshesToRender.at(numActive) = &entity.GetComponent<Okay::MeshComponent>();
	transforms.at(numActive) = &entity.GetComponent<Okay::TransformComponent>();
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
		DX11::UpdateBuffer(pWorldBuffer, &transforms.at(i)->matrix, sizeof(DirectX::XMFLOAT4X4));
		meshesToRender.at(i)->mesh->Bind();
		meshesToRender.at(i)->mesh->Draw();
	}
}

void Renderer::Bind()
{
	pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDevContext->IASetInputLayout(pInputLayout);

	pDevContext->VSSetShader(pVertexShader, nullptr, 0);
	pDevContext->VSSetConstantBuffers(0, 1, &pViewProjectBuffer);
	pDevContext->VSSetConstantBuffers(1, 1, &pWorldBuffer);

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
