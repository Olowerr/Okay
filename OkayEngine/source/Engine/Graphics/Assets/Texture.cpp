#include "Texture.h"

namespace Okay
{
	Texture::Texture(const unsigned char* pData, uint32 width, uint32 height, std::string_view name)
		:name(name), width(width), height(height)
	{
		D3D11_TEXTURE2D_DESC desc = createDefaultDesc();
		D3D11_SUBRESOURCE_DATA initData{pData, width * 4, 0};

		DX11& dx11 = DX11::getInstance();
		dx11.getDevice()->CreateTexture2D(&desc, &initData, &texture);
		if (!texture)
			return;

		dx11.getDevice()->CreateShaderResourceView(texture, nullptr, &srv);
	}

	Texture::~Texture()
	{
		shutdown();
	}

	void Texture::shutdown()
	{
		DX11_RELEASE(texture);
		DX11_RELEASE(srv);
	}
}
