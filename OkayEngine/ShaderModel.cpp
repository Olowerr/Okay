#include "ShaderModel.h"

Okay::ShaderModel::ShaderModel(bool defaultShader)
	:pPixelShader(), pComputeShader()
{
	if (defaultShader)
		CreateDefaultPS();
	
}

bool Okay::ShaderModel::CreateDefaultIL()
{
	std::string shaderData;
	D3D11_INPUT_ELEMENT_DESC desc[3] = {
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV",			0, DXGI_FORMAT_R32G32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	
	VERIFY(Okay::ReadShader("MeshVS.cso", shaderData));
	VERIFY_HR(DX11::Get().GetDevice()->CreateInputLayout(desc, 3, shaderData.c_str(), shaderData.length(), &pInputLayout));

	return true;
}

bool Okay::ShaderModel::CreateDefaultVS()
{
	std::string shaderData;

	VERIFY(Okay::ReadShader("MeshVS.cso", shaderData));
	VERIFY_HR(DX11::Get().GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &pVertexShader));

	return true;
}

bool Okay::ShaderModel::CreateDefaultHS()
{
	return true; // TODO: Implement Tessellation
}

bool Okay::ShaderModel::CreateDefaultDS()
{
	return true; // TODO: Implement Tessellation
}

bool Okay::ShaderModel::CreateDefaultPS()
{
	std::string shaderData;

	VERIFY(Okay::ReadShader("MeshVS.cso", shaderData));
	VERIFY_HR(DX11::Get().GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &pVertexShader));

	return true;
}
