#pragma once
#include "Engine/DirectX/DX11.h"

#include <string_view>

namespace Okay
{
	class Texture;

	class Shader
	{
	public:

		struct GPUData
		{
			BOOL hasHeightMap = FALSE; // BOOL is a typedef of int cuz normal bool is too small
			float heightMapScalar = 1.f;

			int padding[2]{};
		};

		inline void setCustomShaderPath(std::string_view location);

		Shader(bool ignoreCustomShaderPath = false);
		Shader(std::string_view name, std::string_view shaderPath, bool ignoreCustomShaderPath = false);
		Shader(Shader&& other) noexcept;
		~Shader();
		void shutdown();

		inline const std::string& getName() const;
		inline void setName(std::string_view name);

		void setHeightMap(uint32_t index = Okay::INVALID_UINT);
		inline uint32_t getHeightMapID() const;
		const Texture* getHeightMap() const;
		inline void setHeightMapScalar(float scalar);

		void setPixelShader(std::string_view path);
		void reloadShader();
		inline const std::string& getPSName() const;
		
		void bind(ID3D11DeviceContext* pDefContext) const;
		inline const GPUData& getGPUData() const;

	private:
		std::string name;

		GPUData gpuData;

		const bool ignoreCustomShaderPath; // Might change, currently used for Engine shaders (like PhongPS)
		std::string psName;
		ID3D11PixelShader* pPS;

		ID3D11ShaderResourceView* pHeightMap;
		uint32_t heightMapIdx;

		static std::string shaderLocation;
		void createDefaultShader();
	};

	inline void Shader::setCustomShaderPath(std::string_view location)
	{
		Shader::shaderLocation = location;
	}

	inline const std::string& Shader::getName() const
	{
		return name;
	}

	inline void Shader::setName(std::string_view name)
	{
		this->name = name;
	}

	inline const std::string& Shader::getPSName() const
	{
		return psName;
	}

	inline uint32_t Shader::getHeightMapID() const
	{
		return heightMapIdx;
	}

	inline const Shader::GPUData& Shader::getGPUData() const
	{
		return gpuData;
	}

	void Shader::setHeightMapScalar(float scalar)
	{
		gpuData.heightMapScalar = scalar;
	}

}