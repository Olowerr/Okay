#include "Shader.h"
#include "Engine/DirectX/DX11.h"

#include <fstream>

namespace Okay
{
	const std::string Shader::ShaderPath = "../OkayEngine/engine_resources/shaders/bin/";

	bool Shader::readShader(std::string_view shaderName, std::string& output)
	{
		std::ifstream reader(ShaderPath + shaderName.data() + ".cso", std::ios::binary);
		OKAY_VERIFY(reader);

		reader.seekg(0, std::ios::end);
		output.reserve((size_t)reader.tellg());
		reader.seekg(0, std::ios::beg);

		output.assign(std::istreambuf_iterator<char>(reader), std::istreambuf_iterator<char>());

		return true;
	}

	Shader::Shader()
		:name("Default"), pPS(nullptr)
	{
		setPixelShader("PhongPS");
	}

	Shader::Shader(std::string_view name)
		:name(name), pPS(nullptr)
	{
		setPixelShader("PhongPS");
	}

	Shader::Shader(std::string_view psPath, std::string_view name)
		:name(name), pPS(nullptr)
	{
		setPixelShader(psPath);
	}

	Shader::Shader(Shader&& other) noexcept
		:name(std::move(other.name))
	{
		pPS = other.pPS;
		other.pPS = nullptr;
	}

	Shader::~Shader()
	{
		shutdown();
	}

	void Shader::shutdown()
	{
		DX11_RELEASE(pPS);
	}

	void Shader::bind() const
	{
		ID3D11DeviceContext* pDevCon = DX11::getInstance().getDeviceContext();

		pDevCon->PSSetShader(pPS, nullptr, 0);
	}

	void Shader::setPixelShader(std::string_view path)
	{
		DX11_RELEASE(pPS);

		std::string shaderData;

		bool foundShader = Shader::readShader(path, shaderData);
		OKAY_ASSERT(foundShader, "Failed to read shader");

		HRESULT hr = DX11::getInstance().getDevice()->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pPS);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed creating pixel shader");


		size_t pos1 = path.find_last_of('/');
		pos1 = pos1 == std::string_view::npos ? path.find_last_of('\\') : pos1;
		pos1 = pos1 == std::string_view::npos ? 0ull : pos1;

		psName = path.substr(pos1);
	}

}

