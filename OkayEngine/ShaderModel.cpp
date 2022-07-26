#include "ShaderModel.h"

Okay::ShaderModel::ShaderModel(bool defaultShader)
	:pPixelShader(), pComputeShader(), viewport()
{
	if (defaultShader)
	{
		CreateDefaultPS();

		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MaxDepth = 1.f;
		viewport.MinDepth = 0.f;
		viewport.Width = (FLOAT)DX11::Get().GetWindowWidth();
		viewport.Height = (FLOAT)DX11::Get().GetWindowHeight();
	}
}

Okay::ShaderModel::~ShaderModel()
{
	Shutdown();
}

void Okay::ShaderModel::Shutdown()
{
	DX11_RELEASE(pPixelShader);
	DX11_RELEASE(pComputeShader);
}

void Okay::ShaderModel::Bind()
{
	DX11::Get().GetDeviceContext()->PSSetShader(pPixelShader, nullptr, 0);
	DX11::Get().GetDeviceContext()->RSSetViewports(1, &viewport);
	DX11::Get().GetDeviceContext()->OMSetRenderTargets(1, DX11::Get().GetMainRTV(), *DX11::Get().GetDepthBufferDSV());
}

void Okay::ShaderModel::UnBind()
{
	ID3D11RenderTargetView* nullRTV = nullptr;
	ID3D11DepthStencilView* nullDSV = nullptr;
	DX11::Get().GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);
	DX11::Get().GetDeviceContext()->OMSetRenderTargets(1, &nullRTV, nullDSV);
}

void Okay::ShaderModel::Apply()
{
	// Post process for derived Shader Models
}

void Okay::ShaderModel::Resize()
{
	viewport.Width = (FLOAT)DX11::Get().GetWindowWidth();
	viewport.Height = (FLOAT)DX11::Get().GetWindowHeight();
	Bind();
}

bool Okay::ShaderModel::CreateDefaultPS()
{
	std::string shaderData;

	VERIFY(Okay::ReadShader("PhongPS.cso", shaderData));
	VERIFY_HR(DX11::Get().GetDevice()->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pPixelShader));

	return true;
}
