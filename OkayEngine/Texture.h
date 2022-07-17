#pragma once
#include "DX11.h"

namespace Okay
{
	class Texture
	{
	public:
		Texture();
		Texture(UINT width, UINT height, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlags);
		Texture(const String& path, D3D11_BIND_FLAG bindFlags = D3D11_BIND_SHADER_RESOURCE);
		Texture(void* pData, UINT width, UINT height, D3D11_BIND_FLAG bindFlags = D3D11_BIND_SHADER_RESOURCE);

		~Texture();
		void Shutdown();

		ID3D11Texture2D* const* GetTexture();
		ID3D11ShaderResourceView* const* GetSRV();
		ID3D11RenderTargetView* const* GetRTV();
		ID3D11UnorderedAccessView* const* GetUAV();

	private:
		ID3D11Texture2D* texture;
		ID3D11ShaderResourceView* srv;
		ID3D11RenderTargetView* rtv;
		ID3D11UnorderedAccessView* uav;
	};
}