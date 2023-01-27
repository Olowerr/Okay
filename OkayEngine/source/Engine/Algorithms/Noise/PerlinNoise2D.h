#pragma once

#include "Engine/Okay/Okay.h"
#include "Engine/DirectX/DX11.h"

namespace Okay
{
	class PerlinNoise2D
	{
	public:
		PerlinNoise2D(uint32_t seed = 0);
		~PerlinNoise2D();

		inline void setSeed(uint32_t seed);
		inline void setOctaves(uint32_t octaves);
		inline void setSections(uint32_t sections);
		inline void setOctWidth(uint32_t octWidth);
		inline void setFrequency(glm::vec2 frequency);
		inline void setBias(float bias);

		float sample(float x, float y);
		float sample2(float x, float y);
		float sample3(float x, float y);
		void generateTexture(ID3D11Texture2D* output);

		bool imgui(const char* label);

	private:
		uint32_t seed;

		uint32_t octaves;
		uint32_t sections;
		uint32_t startOctWidth;

		glm::vec2 frequency;
		float bias;
		
		bool guiLockFreqRatio;

		void createResources(ID3D11Texture2D* output, ID3D11Texture2D** resultBuffer, uint32_t* width, uint32_t* height);
		float sampleSeed(int x, int y);

		inline float toon(float value);
	};

	inline void PerlinNoise2D::setSeed(uint32_t seed)
	{
		this->seed = seed;
	}

	inline float PerlinNoise2D::toon(float value)
	{
		return value - std::fmod(value, (1.f / (float)sections));
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
	
	inline void PerlinNoise2D::setFrequency(glm::vec2 frequency)
	{
		this->frequency = frequency;
	}
}