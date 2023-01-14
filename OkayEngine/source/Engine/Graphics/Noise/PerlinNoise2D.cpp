#include "PerlinNoise2D.h"

namespace Okay
{
	PerlinNoise2D::PerlinNoise2D(uint32_t octaves, uint32_t sections, float bias, uint32_t startOctWidth)
		:seed(0u), octaves(octaves), sections(sections), bias(bias), startOctWidth(startOctWidth)
	{
	}

	PerlinNoise2D::~PerlinNoise2D()
	{
		shutdown();
	}

	void PerlinNoise2D::shutdown()
	{
		
	}

	void PerlinNoise2D::generateTexture(ID3D11Texture2D* output)
	{
		uint32_t width = 0u, height = 0u;
		ID3D11Texture2D* resultBuffer = nullptr;
		createResources(output, &resultBuffer, &width, &height);

		unsigned char* result = new unsigned char[(size_t)width * (size_t)height]{};

		for (uint32_t x = 0; x < width; x++)
		{
			for (uint32_t y = 0; y < height; y++)
			{
				result[width * y + x] = sample_Internal(x, y, width, height);
			}
		}

		DX11::updateTexture(resultBuffer, result, 1u, width, height);
		DX11::getInstance().getDeviceContext()->CopyResource(output, resultBuffer);

		DX11_RELEASE(resultBuffer);
		OKAY_DELETE_ARRAY(result);
	}
	
	unsigned char PerlinNoise2D::sample_Internal(int x, int y, int width, int height)
	{
		float noise = 0;
		float scale = 1.f;
		float scaleAcc = 0.f;

		for (uint32_t o = 0; o < octaves; o++)
		{
#if 1 // My version
			int pitchX = width >> o;
			int pitchY = height >> o;
			if (!pitchX) pitchX = 1u;
			if (!pitchY) pitchY = 1u;

			const int sampleX1 = (x / pitchX) * pitchX;
			const int sampleY1 = (y / pitchY) * pitchY;
				  
			const int sampleX2 = (sampleX1 + pitchX);
			const int sampleY2 = (sampleY1 + pitchY);

			const float lerpTX = (float)(x % pitchX) / (float)pitchX;
			const float lerpTY = (float)(y % pitchY) / (float)pitchY;

			const float blendX1 = glm::mix(sampleSeed(sampleX1, sampleY1), sampleSeed(sampleX2, sampleY1), lerpTX);
			const float blendX2 = glm::mix(sampleSeed(sampleX1, sampleY2), sampleSeed(sampleX2, sampleY2), lerpTX);

			scaleAcc += scale;
			noise += glm::mix(blendX1, blendX2, lerpTY) * scale;
			scale /= bias;

#else // OneLoneCoder/Javidx9's version
			int nPitch = startWidth >> o;
			int nSampleX1 = (x / nPitch) * nPitch;
			int nSampleY1 = (y / nPitch) * nPitch;

			int nSampleX2 = (nSampleX1 + nPitch) % width;
			int nSampleY2 = (nSampleY1 + nPitch) % width;

			float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
			float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

			float fSampleT = (1.0f - fBlendX) * sampleSeed(nSampleX1, nSampleY1) + fBlendX * sampleSeed(nSampleX2, nSampleY1);
			float fSampleB = (1.0f - fBlendX) * sampleSeed(nSampleX1, nSampleY2) + fBlendX * sampleSeed(nSampleX2, nSampleY2);

			scaleAcc += scale;
			noise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * scale;
			scale = scale / bias;
#endif
		}

		return toon(UNORM_TO_UCHAR(noise / scaleAcc), sections);
	}

	float PerlinNoise2D::sampleSeed(int x, int y)
	{
		x = std::abs(x);
		y = std::abs(y);

		/* mix around the bits in x: */
		x += seed;
		x = x * 3266489917 + 374761393;
		x = (x << 17) | (x >> 15);

		/* mix around the bits in y and mix those into x: */
		x += y * 3266489917;

		/* Give x a good stir: */
		x *= 668265263;
		x ^= x >> 15;
		x *= 2246822519;
		x ^= x >> 13;
		x *= 3266489917;
		x ^= x >> 16;
		
		/* trim the result and scale it to a float in [0,1): */
		return (x & 0x00ffffff) * (1.0f / 0x1000000);
	}

	void PerlinNoise2D::createResources(ID3D11Texture2D* output, ID3D11Texture2D** resultBuffer, uint32_t* width, uint32_t* height)
	{
		D3D11_TEXTURE2D_DESC texDesc{};
		output->GetDesc(&texDesc);
		*width = texDesc.Width;
		*height = texDesc.Height;

		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		texDesc.Format = DXGI_FORMAT_R8_UNORM;
		texDesc.ArraySize = 1u;
		texDesc.MipLevels = 1u;
		texDesc.SampleDesc.Count = 1u;
		texDesc.SampleDesc.Quality = 0u;
		texDesc.MiscFlags = 0u;
		texDesc.Usage = D3D11_USAGE_DEFAULT;

		DX11::getInstance().getDevice()->CreateTexture2D(&texDesc, nullptr, resultBuffer);
	}
}
