#include "Shader.h"
#include "Engine/DirectX/DX11.h"

#include <fstream>

namespace Okay
{
	const std::string Shader::ShaderPath = "../OkayEngine/resources/shaders/bin/";

	bool Shader::readShader(std::string_view shaderName, std::string& output)
	{
		std::ifstream reader(ShaderPath + shaderName.data(), std::ios::binary);
		OKAY_VERIFY(reader);

		reader.seekg(0, std::ios::end);
		output.reserve((size_t)reader.tellg());
		reader.seekg(0, std::ios::beg);

		output.assign(std::istreambuf_iterator<char>(reader), std::istreambuf_iterator<char>());

		return true;
	}

	Shader::Shader(std::string_view path)
	{
		std::string shaderData;
		bool foundShader = Shader::readShader(path, shaderData);
		OKAY_ASSERT(foundShader, "Failed to read shader");
	
		DX11& dx11 = DX11::getInstance();
		HRESULT hr = dx11.getDevice()->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pPixelShader);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed creating pixel shader");
	}

	Shader::~Shader()
	{
		shutdown();
	}

	void Shader::shutdown()
	{
		DX11_RELEASE(pPixelShader);
	}

	void Shader::bind()
	{
		ID3D11DeviceContext* pDevCon = DX11::getInstance().getDeviceContext();
		pDevCon->PSSetShader(pPixelShader, nullptr, 0);
	}

	//void Shader::apply()
	//{
	//	// Post process for derived Shader Models
	//}
}

