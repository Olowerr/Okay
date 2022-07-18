#include "Texture.h"

Okay::Texture::Texture()
	:name("Quack"), texture(), srv(), rtv(), uav(), width(0), height(0), isValid(false)
{
	unsigned char* pData = nullptr;
	if (!LoadTexture("../Content/Images/Quack.jpg", &pData))
		return;

	D3D11_TEXTURE2D_DESC desc = CreateDefaultDesc();
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA inData{pData, width * 4, 0};

	DX11::Get().GetDevice()->CreateTexture2D(&desc, &inData, &texture);
	
	stbi_image_free(pData);
	if (!texture)
		return;

	DX11::Get().GetDevice()->CreateShaderResourceView(texture, nullptr, &srv);
	
	isValid = srv;
}

Okay::Texture::Texture(UINT width, UINT height, DXGI_FORMAT format, UINT bindFlags, const Okay::String& name)
	:isValid(false), width(width), height(height), name(name), texture(), srv(), rtv(), uav()
{
	D3D11_TEXTURE2D_DESC desc = CreateDefaultDesc();

	desc.Format = format;
	desc.BindFlags = bindFlags;

	DX11::Get().GetDevice()->CreateTexture2D(&desc, nullptr, &texture);
	if (!texture)
		return;

	CreateViews(bindFlags);
}

Okay::Texture::Texture(const std::string& path, UINT bindFlags)
	:isValid(false), name(path.substr(path.find_last_of('/') + 1)), texture(), srv(), rtv(), uav()
{
	unsigned char* pData = nullptr;
	if (!LoadTexture(path, &pData))
		return;

	D3D11_TEXTURE2D_DESC desc = CreateDefaultDesc();
	desc.BindFlags = bindFlags;
	D3D11_SUBRESOURCE_DATA inData{ pData, width * 4, 0 };

	DX11::Get().GetDevice()->CreateTexture2D(&desc, &inData, &texture);

	stbi_image_free(pData);
	if (!texture)
		return;

	CreateViews(bindFlags);
}

Okay::Texture::~Texture()
{
	Shutdown();
}

void Okay::Texture::Shutdown()
{
	DX11_RELEASE(texture);
	DX11_RELEASE(srv);
	DX11_RELEASE(rtv);
	DX11_RELEASE(uav);
	isValid = false;
}

bool Okay::Texture::LoadTexture(const std::string& path, unsigned char** ppData)
{
	int x, y, c;
	unsigned char* pData = stbi_load(path.c_str(), &x, &y, &c, 4);

	if (!pData)
		return false;

	width = (UINT)x;
	height = (UINT)y;
	*ppData = pData;

	return true;
}

void Okay::Texture::CreateViews(UINT bindFlags)
{
	isValid = true;

	if ((bindFlags | D3D11_BIND_SHADER_RESOURCE) == bindFlags)
	{
		DX11::Get().GetDevice()->CreateShaderResourceView(texture, nullptr, &srv);
		isValid = srv;
	}

	if ((bindFlags | D3D11_BIND_RENDER_TARGET) == bindFlags)
	{
		DX11::Get().GetDevice()->CreateRenderTargetView(texture, nullptr, &rtv);
		isValid = rtv && isValid;
	}

	if ((bindFlags | D3D11_BIND_UNORDERED_ACCESS) == bindFlags)
	{
		DX11::Get().GetDevice()->CreateUnorderedAccessView(texture, nullptr, &uav);
		isValid = uav && isValid;
	}
}
