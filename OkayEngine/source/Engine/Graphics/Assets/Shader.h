#pragma once
#include "Engine/DirectX/DX11.h"

#include <string_view>

namespace Okay
{
	class ContentBrowser;
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

		static const std::string ShaderPath;
		static bool readShader(std::string_view shaderName, std::string& output);

		Shader(const ContentBrowser& content);
		Shader(const ContentBrowser& content, std::string_view name);
		Shader(const ContentBrowser& content, std::string_view psPath, std::string_view name);
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
		void compilePixelShader(std::string_view path);
		void reloadShader();
		inline const std::string& getPSName() const;
		
		void bind() const;
		inline const GPUData& getGPUData() const;

	private:
		const ContentBrowser& content;
		std::string name;

		GPUData gpuData;

		std::string psName;
		ID3D11PixelShader* pPS;

		ID3D11ShaderResourceView* pHeightMap;
		uint32_t heightMapIdx;
	};

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

/*

	Renderer ALWAYS sends the same type of data to PS
	ShaderModel decides how to use the data


	MESH CLASS
	MESH CONTAINER


*/