#include "SkyBox.h"

#include "ContentBrowser.h"
#include "Engine/Graphics/Assets/Importing/stb_image.h"

namespace Okay
{
	SkyBox::SkyBox()
		:pTextureCube(nullptr), pTextureCubeSRV(nullptr)
	{
	}

	SkyBox::~SkyBox()
	{
		shutdown();
	}

	void SkyBox::shutdown()
	{
		DX11_RELEASE(pTextureCube);
		DX11_RELEASE(pTextureCubeSRV);
	}

	bool SkyBox::create(std::string_view path)
	{
		int imgWidth = 0, imgHeight = 0;

		// Using a 4 byte type ptr so one "step" is one pixel instead of one colour value
		uint32_t* pImgData = (uint32_t*)stbi_load(path.data(), &imgWidth, &imgHeight, nullptr, 4);
		if (!pImgData)
			return false;

		if (imgWidth % 4 != 0 || imgHeight % 3 != 0)
		{
			stbi_image_free(pImgData);
			return false;
		}

		const uint32_t width = imgWidth / 4;
		const uint32_t height = imgHeight / 3;
		const uint32_t byteWidth = width * height * 4u;

		D3D11_SUBRESOURCE_DATA data[6]{};
		for (size_t i = 0; i < 6; i++)
		{
			data[i].pSysMem = malloc(byteWidth);
			data[i].SysMemPitch = uint32_t(width * 4u);
			data[i].SysMemSlicePitch = 0u;
		}
	
		// The coursor points to the location of each side
		uint32_t* coursor = nullptr;

		// Positive X
		coursor = pImgData + imgWidth * height + width * 2u;
		copyImgSection((uint32_t*)data[0].pSysMem, coursor, imgWidth, width, height);

		// Negative X
		coursor = pImgData + imgWidth * height;
		copyImgSection((uint32_t*)data[1].pSysMem, coursor, imgWidth, width, height);

		// Positive Y
		coursor = pImgData + width;
		copyImgSection((uint32_t*)data[2].pSysMem, coursor, imgWidth, width, height);

		// Negative Y
		coursor = pImgData + imgWidth * height * 2u + width;
		copyImgSection((uint32_t*)data[3].pSysMem, coursor, imgWidth, width, height);

		// Positive Z
		coursor = pImgData + imgWidth * height + width * 3u;
		copyImgSection((uint32_t*)data[4].pSysMem, coursor, imgWidth, width, height);
		
		// Negative Z
		coursor = pImgData + imgWidth * height + width;
		copyImgSection((uint32_t*)data[5].pSysMem, coursor, imgWidth, width, height);


		D3D11_TEXTURE2D_DESC desc{};
		desc.ArraySize = 6u;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0u;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;

		HRESULT hr = DX11::get().getDevice()->CreateTexture2D(&desc, data, &pTextureCube);

		stbi_image_free(pImgData);
		for (size_t i = 0; i < 6; i++)
			free((void*)data[i].pSysMem);

		OKAY_ASSERT(SUCCEEDED(hr), "Failed creating Texture Cube");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = 1u;
		srvDesc.TextureCube.MostDetailedMip = 0u;

		hr = DX11::get().getDevice()->CreateShaderResourceView(pTextureCube, &srvDesc, &pTextureCubeSRV);
		if (FAILED(hr))
			pTextureCube->Release();

		OKAY_ASSERT(SUCCEEDED(hr), "Failed creating Texture Cube SRV");

		DX11::get().getDeviceContext()->PSSetShaderResources(10, 1, &pTextureCubeSRV);

		return true;
	}
	
	void SkyBox::copyImgSection(uint32_t* pTarget, uint32_t* pSource, uint32_t imgWidth, uint32_t readWidth, uint32_t readHeight)
	{
		for (uint32_t i = 0; i < readHeight; i++)
		{
			memcpy(pTarget, pSource, readWidth * 4ull);
			pTarget += readWidth;
			pSource += imgWidth;
		}
	}
}
