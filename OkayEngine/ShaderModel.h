#pragma once
#include "DX11.h"

namespace Okay
{
	class ShaderModel
	{
	public:
		ShaderModel(bool defaultShader = false);
		virtual ~ShaderModel();
		virtual void Shutdown();
	
		virtual void Bind();
		virtual void Apply();
		void Resize();

	protected:
		D3D11_VIEWPORT viewport;
		ID3D11PixelShader* pPixelShader;
		ID3D11ComputeShader* pComputeShader;
		bool CreateDefaultPS();

	};
}

/*

	Renderer ALWAYS sends the same type of data to PS
	ShaderModel decides how to use the data


	MESH CLASS
	MESH CONTAINER


*/