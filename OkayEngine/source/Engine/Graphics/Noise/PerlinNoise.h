#pragma once

#include "Engine/Okay/Okay.h"
#include "Engine/DirectX/DX11.h"

namespace Okay
{
	class PerlinNoise2D
	{
	public:
		PerlinNoise2D(ID3D11Texture2D* output);
		~PerlinNoise2D();
		void shutdown();

		void randomizeSeed(float scale = 1.f);
		void generate();

		ID3D11ShaderResourceView* seedBufferSRV;
	private:

		unsigned char* seed;
		ID3D11Texture2D* seedBuffer;

		ID3D11UnorderedAccessView* uav;

		void createSeedResources(ID3D11Texture2D* output);
		void randomizeSeed_Internal(uint32_t width, uint32_t height, float scale);
	};
}