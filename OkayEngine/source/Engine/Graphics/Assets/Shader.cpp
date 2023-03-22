
#include "Shader.h"
#include "Engine/DirectX/DX11.h"
#include "../ContentBrowser.h"

#include <d3dcompiler.h>
#include <stdlib.h>
#include <fstream>

namespace Okay
{
	// No location forces them to create whereever the working directory is
	// Is better than SHADER_PATH no ?
	std::string Shader::shaderLocation = "";

	Shader::Shader(bool ignoreCustomShaderPath)
		:name("New Shader"), pPS(nullptr), pHeightMap(nullptr), heightMapIdx(Okay::INVALID_UINT)
		, ignoreCustomShaderPath(ignoreCustomShaderPath)
	{
		createDefaultShader();
		DX11::createShader(shaderLocation + psName, &pPS);
	}

	Shader::Shader(std::string_view name, std::string_view shaderPath, bool ignoreCustomShaderPath)
		:name(name), pPS(nullptr), pHeightMap(nullptr), heightMapIdx(Okay::INVALID_UINT)
		, ignoreCustomShaderPath(ignoreCustomShaderPath)
	{
		setPixelShader(shaderPath);
	}

	Shader::Shader(Shader&& other) noexcept
		:name(std::move(other.name)), psName(std::move(other.psName)),
		gpuData(std::move(other.gpuData)), ignoreCustomShaderPath(other.ignoreCustomShaderPath)
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

		// TODO: (Temp before asset saving) delete the created shader
	}

	void Shader::shutdown()
	{
		DX11_RELEASE(pPS);
		DX11_RELEASE(pHeightMap);
		heightMapIdx = Okay::INVALID_UINT;
	}

	void Shader::bind() const
	{
		ID3D11DeviceContext* pDevCon = DX11::get().getDeviceContext();
		pDevCon->VSSetShaderResources(2u, 1u, &pHeightMap);
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
		pHeightMap = ContentBrowser::get().getTexture(index).getSRV();
		pHeightMap->AddRef();
	}

	const Texture* Shader::getHeightMap() const
	{
		return heightMapIdx != Okay::INVALID_UINT ? &ContentBrowser::get().getTexture(heightMapIdx) : nullptr;
	}

	void Shader::setPixelShader(std::string_view path)
	{
		ID3D11PixelShader* newPs = nullptr;
		if (!DX11::createShader(path, &newPs))
			return;

		DX11_RELEASE(pPS);
		pPS = newPs;

		size_t pos = findLastSlashPos(path);
		pos = pos == std::string_view::npos ? 1ull : ++pos;
		psName = path.substr(pos);
	}

	void Shader::reloadShader()
	{
		ID3D11PixelShader* newPs = nullptr;
		if (!DX11::createShader((ignoreCustomShaderPath ? SHADER_PATH : shaderLocation) + psName, &newPs))
			return;

		DX11_RELEASE(pPS);
		pPS = newPs;
	}

	void Shader::createDefaultShader()
	{
		// Try to find an available file name, so we don't overwrite an existing one
		std::string counterStr = "";
		uint32_t tryCounter = 0;
		while (tryCounter++ < 100u) // :P
		{
			std::ifstream reader(shaderLocation + "NewShader" + counterStr + ".hlsl");
			if (!reader)
			{
				// File name available
				reader.close();
				break;
			}

			reader.close();
			counterStr = counterStr == "" ? "1" : std::to_string(std::stoi(counterStr) + 1);
		}

		OKAY_ASSERT(tryCounter < 100u, "Too many shader creation attempts");

		std::ifstream reader(SHADER_PATH "NewShaderDefault.hlsl");
		OKAY_ASSERT(reader.is_open(), "Failed opening NewShaderDefault.hlsl");

		std::ofstream writer(shaderLocation + "NewShader" + counterStr + ".hlsl");
		OKAY_ASSERT(writer.is_open(), "Failed opening the target shader"); // Would be kinda weird if failed no?

		psName = "NewShader" + counterStr + ".hlsl";

		// Copy the contents of NewShaderDefault
		std::string buffer;
		reader.seekg(0, std::ios::end);
		buffer.reserve((size_t)reader.tellg());
		reader.seekg(0, std::ios::beg);

		while (!reader.eof())
			buffer += reader.get();

		buffer.pop_back();
		writer << buffer;

		reader.close();
		writer.close();
	}
}

