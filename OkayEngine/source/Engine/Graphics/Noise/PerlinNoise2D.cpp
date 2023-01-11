#include "PerlinNoise2D.h"

namespace Okay
{
	PerlinNoise2D::PerlinNoise2D(ID3D11Texture2D* output)
		:seed(nullptr), uav(nullptr)
		,width(0u), height(0u), output(output), result(nullptr), resultBuffer(nullptr)
	{
		OKAY_ASSERT(output, "Invalid output");
		if (!output) 
			return;

		DX11& dx11 = DX11::getInstance();
		dx11.getDevice()->CreateUnorderedAccessView(output, nullptr, &uav);
		OKAY_ASSERT(uav, "Error creating UAV, check DX11 error message");

		createResources(output);
		randomizeSeed();
	}

	PerlinNoise2D::~PerlinNoise2D()
	{
		shutdown();
	}

	void PerlinNoise2D::shutdown()
	{
		DX11_RELEASE(uav);

		DX11_RELEASE(resultBuffer);

		OKAY_DELETE_ARRAY(seed);
		OKAY_DELETE_ARRAY(result);
	}

	void PerlinNoise2D::randomizeSeed(float scale)
	{
		for (uint32_t i = 0; i < width * height; i++)
			seed[i] = UNORM_TO_UCHAR((float)rand() / (float)RAND_MAX);
	}
	
	void PerlinNoise2D::generate(uint32_t octaves, uint32_t sections, float bias)
	{
		for (uint32_t x = 0; x < width; x++)
		{
			for (uint32_t y = 0; y < width; y++)
			{
				float noise = 0;
				float scale = 1.f;
				float scaleAcc = 0.f;

				for (uint32_t o = 0; o < octaves; o++)
				{
#if 1 // My version
					uint32_t pitchX = width >> o;
					uint32_t pitchY = width >> o;
					if (!pitchX) pitchX = 1u;
					if (!pitchY) pitchY = 1u;

					const uint32_t sampleX1 = (x / pitchX) * pitchX;
					const uint32_t sampleY1 = (y / pitchY) * pitchY;

					const uint32_t sampleX2 = (sampleX1 + pitchX) % width;
					const uint32_t sampleY2 = (sampleY1 + pitchY) % height;

					const float lerpTX = (float)(x % pitchX) / (float)pitchX;
					const float lerpTY = (float)(y % pitchY) / (float)pitchY;

					const float blendX1 = glm::mix(sampleSeed(sampleX1, sampleY1), sampleSeed(sampleX2, sampleY1), lerpTX);
					const float blendX2 = glm::mix(sampleSeed(sampleX1, sampleY2), sampleSeed(sampleX2, sampleY2), lerpTX);

					scaleAcc += scale;
					noise += glm::mix(blendX1, blendX2, lerpTY) * scale;
					scale /= bias;

#else // OneLoneCoder/Javidx9's version
					int nPitch = width >> o;
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

				writeResult(x, y, toon(UNORM_TO_UCHAR(noise / scaleAcc), sections));
			}
		}

		DX11::updateTexture(resultBuffer, result, 1u, width, height);
		DX11::getInstance().getDeviceContext()->CopyResource(output, resultBuffer);
	}
	
	void PerlinNoise2D::createResources(ID3D11Texture2D* output)
	{
		D3D11_TEXTURE2D_DESC texDesc{};
		output->GetDesc(&texDesc);
		width = texDesc.Width;
		height = texDesc.Height;

		seed	= new unsigned char[(size_t)width * (size_t)height]{};
		result	= new unsigned char[(size_t)width * (size_t)height]{};

		D3D11_TEXTURE2D_DESC bufferDesc{};
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.Format = DXGI_FORMAT_R8_UNORM;
		bufferDesc.ArraySize = 1u;
		bufferDesc.MipLevels = 1u;
		bufferDesc.SampleDesc.Count = 1u;
		bufferDesc.SampleDesc.Quality = 0u;
		bufferDesc.Height = height;
		bufferDesc.Width = width;
		bufferDesc.MiscFlags = 0u;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;

		DX11::getInstance().getDevice()->CreateTexture2D(&bufferDesc, nullptr, &resultBuffer);
	}
}
