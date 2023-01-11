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
		void generate(uint32_t octaves, uint32_t sections, float bias);

	private:
		uint32_t width, height;
		unsigned char* seed;
		unsigned char* result;
		
		ID3D11Texture2D* resultBuffer;

		ID3D11Texture2D* output;
		ID3D11UnorderedAccessView* uav;

		void createResources(ID3D11Texture2D* output);
		inline float sampleSeed(uint32_t x, uint32_t y);
		inline void writeResult(uint32_t x, uint32_t y, unsigned char value);

		static inline unsigned char toon(unsigned char value, uint32_t sections);
	};

	inline float PerlinNoise2D::sampleSeed(uint32_t x, uint32_t y)
	{
		return (float)seed[width * y + x] / (float)UCHAR_MAX;
	}

	inline void PerlinNoise2D::writeResult(uint32_t x, uint32_t y, unsigned char value)
	{
		result[width * y + x] = value;
	}

	unsigned char PerlinNoise2D::toon(unsigned char value, uint32_t sections)
	{
		return (value / (UCHAR_MAX / sections)) * (UCHAR_MAX / sections);
	}
}