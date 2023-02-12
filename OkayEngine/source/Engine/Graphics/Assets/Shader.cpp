
#include "Shader.h"
#include "Engine/DirectX/DX11.h"
#include "../ContentBrowser.h"

#include <d3dcompiler.h>
#include <stdlib.h>
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

	Shader::Shader(const ContentBrowser& content)
		:name("Default"), pPS(nullptr), pHeightMap(nullptr), heightMapIdx(Okay::INVALID_UINT), content(content)
	{
		setPixelShader("PhongPS");
	}

	Shader::Shader(const ContentBrowser& content, std::string_view name)
		:name(name), pPS(nullptr), pHeightMap(nullptr), heightMapIdx(Okay::INVALID_UINT), content(content)
	{
		setPixelShader("PhongPS");
	}

	Shader::Shader(const ContentBrowser& content, std::string_view psPath, std::string_view name)
		:name(name), pPS(nullptr), pHeightMap(nullptr), heightMapIdx(Okay::INVALID_UINT), content(content)
	{
		setPixelShader(psPath);
	}

	Shader::Shader(Shader&& other) noexcept
		:name(std::move(other.name)), content(other.content), psName(std::move(other.psName)),
		gpuData(std::move(other.gpuData))
	{
		pPS = other.pPS;
		other.pPS = nullptr;

		heightMapIdx = other.heightMapIdx;
		other.heightMapIdx = Okay::INVALID_UINT;

		pHeightMap = other.pHeightMap;
		other.pHeightMap = nullptr;
	}

	Shader::~Shader()
	{
		shutdown();
	}

	void Shader::shutdown()
	{
		DX11_RELEASE(pPS);
		DX11_RELEASE(pHeightMap);
		heightMapIdx = Okay::INVALID_UINT;
	}

	void Shader::bind() const
	{
		ID3D11DeviceContext* pDevCon = DX11::getInstance().getDeviceContext();
		pDevCon->VSSetShaderResources(0, 1, &pHeightMap);
		pDevCon->PSSetShader(pPS, nullptr, 0);
	}

	void Shader::setHeightMap(uint32_t index)
	{
		heightMapIdx = index;
		DX11_RELEASE(pHeightMap);

		if (index == Okay::INVALID_UINT)
		{
			gpuData.hasHeightMap = false;
			gpuData.heightMapScalar = 1.f;	
			return;
		}
		
		gpuData.hasHeightMap = TRUE;
		pHeightMap = content.getTexture(index).getSRV();
		pHeightMap->AddRef();
	}

	const Texture* Shader::getHeightMap() const
	{
		return heightMapIdx != Okay::INVALID_UINT ? &content.getTexture(heightMapIdx) : nullptr;
	}

	void Shader::setPixelShader(std::string_view path)
	{
		std::string shaderData;
		bool foundShader = Shader::readShader(path, shaderData);
		OKAY_ASSERT(foundShader, "Failed to read shader");

		ID3D11PixelShader* newPs = nullptr;
		HRESULT hr = DX11::getInstance().getDevice()->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &newPs);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed creating pixel shader");

		DX11_RELEASE(pPS);
		pPS = newPs;

		size_t pos1 = path.find_last_of('/');
		pos1 = pos1 == std::string_view::npos ? path.find_last_of('\\') : pos1;
		pos1 = pos1 == std::string_view::npos ? 0ull : pos1;

		psName = path.substr(pos1);
	}

	void Shader::reloadShader()
	{
		compilePixelShader("resources/Shaders/" + psName);
	}

	void Shader::compilePixelShader(std::string_view path)
	{
		ID3DBlob* outData = nullptr;
		ID3DBlob* outErrors = nullptr;
		
		wchar_t* lpPath = new wchar_t[path.size() + 1ull]{};
		mbstowcs_s(nullptr, lpPath, path.size() + 1ull, path.data(), path.size());

		HRESULT hr = D3DCompileFromFile(lpPath, nullptr, nullptr, "main", "ps_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL2, 0u, &outData, &outErrors);
		OKAY_DELETE_ARRAY(lpPath);

		ID3D11PixelShader* pNewPS = nullptr;
		if (SUCCEEDED(hr))
		{
			DX11::getInstance().getDevice()->CreatePixelShader(outData->GetBufferPointer(), outData->GetBufferSize(), nullptr, &pNewPS);
			DX11_RELEASE(outData);
			if (!pNewPS)
				return;
		}
		else
		{
			if (outErrors)
			{
				printf("Error compiling shader: %s\n", (char*)outErrors->GetBufferPointer());
				DX11_RELEASE(outErrors);
			}

			return;
		}
		
		size_t pos1 = path.find_last_of('/');
		pos1 = pos1 == std::string_view::npos ? path.find_last_of('\\') : pos1;
		pos1 = pos1 == std::string_view::npos ? 0ull : pos1 + 1ull;
		psName = path.substr(pos1);

		DX11_RELEASE(pPS);
		pPS = pNewPS;
	}
}

