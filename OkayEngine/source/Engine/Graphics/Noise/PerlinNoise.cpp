#include "PerlinNoise.h"

namespace Okay
{
	PerlinNoise2D::PerlinNoise2D(ID3D11Texture2D* output)
		:seed(nullptr), uav(nullptr), seedBuffer(nullptr)
	{
		OKAY_ASSERT(output, "Invalid output");
		if (!output) 
			return;

		DX11& dx11 = DX11::getInstance();
		dx11.getDevice()->CreateUnorderedAccessView(output, nullptr, &uav);
		OKAY_ASSERT(uav, "Error creating UAV, check DX11 error message");

		createSeedResources(output);
		randomizeSeed();
	}

	PerlinNoise2D::~PerlinNoise2D()
	{
		shutdown();
	}

	void PerlinNoise2D::shutdown()
	{
		DX11_RELEASE(uav);
		DX11_RELEASE(seedBuffer);
		DX11_RELEASE(seedBufferSRV);
		OKAY_DELETE_ARRAY(seed);
	}

	void PerlinNoise2D::randomizeSeed(float scale)
	{
		ID3D11Resource* resource = nullptr;
		ID3D11Texture2D* texture = nullptr;

		uav->GetResource(&resource);
		
		resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&texture);
		resource->Release();
		OKAY_ASSERT(texture, "The resource was not a Texture2D, check DX11 error messag");

		D3D11_TEXTURE2D_DESC texDesc{};
		texture->GetDesc(&texDesc);
		texture->Release();

		randomizeSeed_Internal(texDesc.Width, texDesc.Height, scale);
	}
	
	void PerlinNoise2D::generate()
	{

	}
	
	void PerlinNoise2D::createSeedResources(ID3D11Texture2D* output)
	{
		D3D11_TEXTURE2D_DESC texDesc{};
		output->GetDesc(&texDesc);
		seed = new unsigned char[(size_t)texDesc.Width * (size_t)texDesc.Height]{};

		D3D11_TEXTURE2D_DESC bufferDesc{};
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.Format = DXGI_FORMAT_R8_UNORM;
		bufferDesc.ArraySize = 1u;
		bufferDesc.MipLevels = 1u;
		bufferDesc.SampleDesc.Count = 1u;
		bufferDesc.SampleDesc.Quality = 0u;
		bufferDesc.Height = texDesc.Height;
		bufferDesc.Width = texDesc.Width;
		bufferDesc.MiscFlags = 0u;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		DX11::getInstance().getDevice()->CreateTexture2D(&bufferDesc, nullptr, &seedBuffer);
		OKAY_ASSERT(seedBuffer, "Failed creating DX11 seed buffer");
		if (!seedBuffer) return; // Gets rid of "seedBuffer could be 0" warning

		DX11::getInstance().getDevice()->CreateShaderResourceView(seedBuffer, nullptr, &seedBufferSRV);

		randomizeSeed_Internal(texDesc.Width, texDesc.Height, 1.f);
	}

	void PerlinNoise2D::randomizeSeed_Internal(uint32_t width, uint32_t height, float scale)
	{
		for (uint32_t i = 0; i < width * height; i++)
			seed[i] = UNORM_TO_UCHAR((float)rand() / (float)RAND_MAX);

		DX11::updateBuffer(seedBuffer, seed, (uint32_t)_msize(seed));
	}
}
