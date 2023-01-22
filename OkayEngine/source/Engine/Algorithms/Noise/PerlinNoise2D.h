#pragma once

#include "Engine/Okay/Okay.h"
#include "Engine/DirectX/DX11.h"

namespace Okay
{
	class PerlinNoise2D
	{
	public:
		PerlinNoise2D(uint32_t octaves = 8u, float bias = 2.f, uint32_t startOctWidth = 512u, uint32_t sections = Okay::INVALID_UINT);
		~PerlinNoise2D();

		inline void setSeed(int seed);
		inline void setOctaves(uint32_t octaves);
		inline void setSections(uint32_t sections);
		inline void setBias(float bias);
		inline void setOctWidth(uint32_t octWidth);

		float sample(float x, float y);
		void generateTexture(ID3D11Texture2D* output);

	private:
		uint32_t octaves;
		uint32_t sections;
		uint32_t startOctWidth;
		float bias;

		int seed;
		
		void createResources(ID3D11Texture2D* output, ID3D11Texture2D** resultBuffer, uint32_t* width, uint32_t* height);
		float sampleSeed(int x, int y);
		float sample_Internal(float x, float y, int width, int height);

		inline float toon(float value);
	};

	inline void PerlinNoise2D::setSeed(int seed)
	{
		this->seed = seed;
	}

	inline float PerlinNoise2D::toon(float value)
	{
		return value - std::fmod(value, (1.f / (float)sections));
	}

	inline float PerlinNoise2D::sample(float x, float y)
	{
		return sample_Internal(x, y, startOctWidth, startOctWidth);
	}

	inline void PerlinNoise2D::setBias(float bias)
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