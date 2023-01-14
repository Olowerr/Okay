#pragma once

#include "Engine/Okay/Okay.h"
#include "Engine/DirectX/DX11.h"

namespace Okay
{
	class PerlinNoise2D
	{
	public:
		PerlinNoise2D(uint32_t octaves, uint32_t sections = 255u, float bias = 2.f, uint32_t startOctWidth = 512);
		~PerlinNoise2D();
		void shutdown();

		inline void setSeed(int seed);
		inline void setOctaves(uint32_t octaves);
		inline void setSections(uint32_t sections);
		inline void setBias(float bias);
		inline void setOctWidth(uint32_t octWidth);
		void generateTexture(ID3D11Texture2D* output);

		float sample(int x, int y);

	private:
		uint32_t octaves;
		uint32_t sections;
		uint32_t startOctWidth;
		float bias;

		int seed;
		
		void createResources(ID3D11Texture2D* output, ID3D11Texture2D** resultBuffer, uint32_t* width, uint32_t* height);
		inline float sampleSeed(int x, int y);
		unsigned char sample_Internal(int x, int y, int width, int height);

		static inline unsigned char toon(unsigned char value, uint32_t sections);
	};

	inline void PerlinNoise2D::setSeed(int seed)
	{
		this->seed = seed;
	}

	inline unsigned char PerlinNoise2D::toon(unsigned char value, uint32_t sections)
	{
		return (value / (UCHAR_MAX / sections)) * (UCHAR_MAX / sections);
	}

	inline float PerlinNoise2D::sample(int x, int y)
	{
		return UCHAR_TO_UNORM(sample_Internal(x, y, startOctWidth, startOctWidth));
	}

	inline void Okay::PerlinNoise2D::setBias(float bias)
	{
		this->bias = bias;
	}

	inline void PerlinNoise2D::setSections(uint32_t sections)
	{
		this->sections = sections;
	}

	inline void Okay::PerlinNoise2D::setOctaves(uint32_t octaves)
	{
		this->octaves = octaves;
	}

	inline void Okay::PerlinNoise2D::setOctWidth(uint32_t octWidth)
	{
		startOctWidth = octWidth;
	}
}