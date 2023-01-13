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

		void setSeed(uint32_t seed);
		void generate(uint32_t octaves, uint32_t sections, float bias);

	private:
		uint32_t width, height;
		uint32_t seed;
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
		/* mix around the bits in x: */
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

	inline void PerlinNoise2D::writeResult(uint32_t x, uint32_t y, unsigned char value)
	{
		result[width * y + x] = value;
	}

	unsigned char PerlinNoise2D::toon(unsigned char value, uint32_t sections)
	{
		return (value / (UCHAR_MAX / sections)) * (UCHAR_MAX / sections);
	}
}