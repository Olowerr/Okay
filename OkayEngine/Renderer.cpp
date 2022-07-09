#include "Renderer.h"

Renderer::Renderer()
	:pInputLayout(), pVertexShader(), pHullShader(), pDomainShader(), pDevContext(DX11::Get().GetDeviceContext())
{
	shaderModel = std::make_unique<Okay::ShaderModel>(true);
	mainCamera = std::make_unique<Okay::Camera>();

	DX11::CreateConstantBuffer(&pViewProjectBuffer, &mainCamera->GetViewProjectMatrix(), sizeof(DirectX::XMFLOAT4X4), false);
	CreateVS();
	CreateHS();
	CreateDS();

	Bind();
	shaderModel->Bind();

	triangle.Bind();

}

Renderer::~Renderer()
{
	Shutdown();
}

void Renderer::Shutdown()
{
	shaderModel->Shutdown();

	DX11_RELEASE(pViewProjectBuffer);

	DX11_RELEASE(pInputLayout);
	DX11_RELEASE(pVertexShader);
	DX11_RELEASE(pHullShader);
	DX11_RELEASE(pDomainShader);
}

void Renderer::Render()
{
	DX11::UpdateBuffer(pViewProjectBuffer, &mainCamera->GetViewProjectMatrix(), sizeof(DirectX::XMFLOAT4X4));
	
	triangle.Draw();
}

void Renderer::Bind()
{
	pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDevContext->IASetInputLayout(pInputLayout);

	pDevContext->VSSetShader(pVertexShader, nullptr, 0);
	pDevContext->VSSetConstantBuffers(0, 1, &pViewProjectBuffer);

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
