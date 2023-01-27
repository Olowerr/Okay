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

		glm::vec2 seedVec[4];

		void createResources(ID3D11Texture2D* output, ID3D11Texture2D** resultBuffer, uint32_t* width, uint32_t* height);
		float sampleSeed(int x, int y);
		inline float toon(float value);

		int hash(int value)
		{
			/* mix around the bits in value: */
			value = value * 3266489917 + 374761393;
			value = (value << 17) | (value >> 15);

			/* Give value a good stir: */
			value *= 668265263;
			value ^= value >> 15;
			value *= 2246822519;
			value ^= value >> 13;
			value *= 3266489917;
			value ^= value >> 16;

			return value;
		}
		glm::vec2 randomGradient(int ix, int iy);
		float dotGridGradient(int ix, int iy, float x, float y);


	};

	inline void PerlinNoise2D::setSeed(uint32_t seed)
	{
		this->seed = seed;

		// temp
		srand(seed);
		seedVec[0] = glm::normalize(glm::vec2((rand() / (float)RAND_MAX) * 2.f - 1.f, (rand() / (float)RAND_MAX) * 2.f - 1.f));
		seedVec[1] = glm::normalize(glm::vec2((rand() / (float)RAND_MAX) * 2.f - 1.f, (rand() / (float)RAND_MAX) * 2.f - 1.f));
		seedVec[2] = glm::normalize(glm::vec2((rand() / (float)RAND_MAX) * 2.f - 1.f, (rand() / (float)RAND_MAX) * 2.f - 1.f));
		seedVec[3] = glm::normalize(glm::vec2((rand() / (float)RAND_MAX) * 2.f - 1.f, (rand() / (float)RAND_MAX) * 2.f - 1.f));
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