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

		void setPixelShader(std::string_view path);
		inline const std::string& getPSName() const;
		
		void bind() const;

	private:
		const ContentBrowser& content;

		struct GPUData
		{
			BOOL hasHeightMap = FALSE; // int but normal byte is too small
		};

		std::string name;

		std::string psName;
		ID3D11PixelShader* pPS;

		ID3D11ShaderResourceView* pHeightMap;
		uint32_t heightMapIdx;
	};

	const std::string& Okay::Shader::getName() const
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
}

/*

	Renderer ALWAYS sends the same type of data to PS
	ShaderModel decides how to use the data


	MESH CLASS
	MESH CONTAINER


*/