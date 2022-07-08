#include "ShaderModel.h"

Okay::ShaderModel::ShaderModel(bool defaultShader)
	:pPixelShader(), pComputeShader()
{
	if (defaultShader)
		CreateDefaultPS();
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
}

void Okay::ShaderModel::Apply()
{
	// Post process for derived Shader Models
}

bool Okay::ShaderModel::CreateDefaultPS()
{
	std::string shaderData;

	VERIFY(Okay::ReadShader("PhongPS.cso", shaderData));
	VERIFY_HR(DX11::Get().GetDevice()->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pPixelShader));

	return true;
}
