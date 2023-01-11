#pragma once
#include "Engine/DirectX/DX11.h"

#include <string_view>

namespace Okay
{
	class Shader
	{
	public:
		static const std::string ShaderPath;
		static bool readShader(std::string_view shaderName, std::string& output);

		Shader();
		Shader(std::string_view name);
		Shader(std::string_view psPath, std::string_view name);
		Shader(Shader&& other) noexcept;
		~Shader();
		void shutdown();

		inline const std::string& getName() const;
		inline void setName(std::string_view name);

		void setPixelShader(std::string_view path);
		inline const std::string& getPSName() const;
		
		void bind() const;

	private:
		std::string name;

		std::string psName;
		ID3D11PixelShader* pPS;

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

}

/*

	Renderer ALWAYS sends the same type of data to PS
	ShaderModel decides how to use the data


	MESH CLASS
	MESH CONTAINER


*/