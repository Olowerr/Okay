#pragma once
#include "DX11.h"

namespace Okay
{
	class ShaderModel
	{
	public:
		ShaderModel(bool defaultShader = false);

	protected:
		ID3D11InputLayout* pInputLayout;
		ID3D11VertexShader* pVertexShader;
		ID3D11HullShader* pHullShader;
		ID3D11DomainShader* pDomainShader;
		ID3D11PixelShader* pPixelShader;

		ID3D11ComputeShader* pComputeShader;

		bool CreateDefaultIL();
		bool CreateDefaultVS();
		bool CreateDefaultHS();
		bool CreateDefaultDS();
		bool CreateDefaultPS();

	};
}



/*

	Renderer ALWAYS sends the same type of data to PS
	ShaderModel decides how to use the data


*/