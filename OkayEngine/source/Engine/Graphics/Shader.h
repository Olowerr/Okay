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

		Shader(std::string_view path);
		~Shader();
		void shutdown();

		void bind();
		//void apply();

	protected:
		ID3D11PixelShader* pPixelShader;
	};

}

/*

	Renderer ALWAYS sends the same type of data to PS
	ShaderModel decides how to use the data


	MESH CLASS
	MESH CONTAINER


*/