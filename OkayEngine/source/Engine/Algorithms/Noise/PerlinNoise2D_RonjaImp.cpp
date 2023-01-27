#include "PerlinNoise2D.h"

namespace Okay
{
	float rand2dTo1d(glm::vec2 value, glm::vec2 dotDir = glm::vec2(12.9898f, 78.233f))
	{
		glm::vec2 smallValue = sin(value);
		float random = glm::dot(smallValue, dotDir);
		random = glm::fract(sin(random) * 143758.5453f);
		return random;
	}

	glm::vec2 rand2dTo2d(glm::vec2 value)
	{
		return glm::vec2(
			rand2dTo1d(value, glm::vec2(12.989f, 78.233f)),
			rand2dTo1d(value, glm::vec2(39.346f, 11.135f)));
	}

	float easeIn(float interpolator)
	{
		return interpolator * interpolator;
	}

	float easeOut(float interpolator)
	{
		return 1 - easeIn(1 - interpolator);
	}

	float easeInOut(float interpolator)
	{
		float easeInValue = easeIn(interpolator);
		float easeOutValue = easeOut(interpolator);
		return glm::mix(easeInValue, easeOutValue, interpolator);
	}

	float PerlinNoise2D::sample3(float x, float y)
	{
		x *= frequency.x * 0.01f;
		y *= frequency.y * 0.01f;

		float noise = 0.f;
		float noiseAcc = 0.f;
		float scale = 1.f;


		for (uint32_t i = 0; i < octaves; i++)
		{
			x /= scale;
			y /= scale;
			
			glm::vec2 fraction(glm::fract(x), glm::fract(y));
			float interpolatorX = easeInOut(fraction.x);
			float interpolatorY = easeInOut(fraction.y);

			//generate random directions
			glm::vec2 lowerLeftDirection = rand2dTo2d(glm::vec2(floor(x), floor(y))) * 2.f - 1.f;
			glm::vec2 lowerRightDirection = rand2dTo2d(glm::vec2(ceil(x), floor(y))) * 2.f - 1.f;
			glm::vec2 upperLeftDirection = rand2dTo2d(glm::vec2(floor(x), ceil(y))) * 2.f - 1.f;
			glm::vec2 upperRightDirection = rand2dTo2d(glm::vec2(ceil(x), ceil(y))) * 2.f - 1.f;


			//get values of cells based on fraction and cell directions
			float lowerLeftFunctionValue = glm::dot(lowerLeftDirection, fraction - glm::vec2(0.f));
			float lowerRightFunctionValue = glm::dot(lowerRightDirection, fraction - glm::vec2(1.f, 0.f));
			float upperLeftFunctionValue = glm::dot(upperLeftDirection, fraction - glm::vec2(0.f, 1.f));
			float upperRightFunctionValue = glm::dot(upperRightDirection, fraction - glm::vec2(1.f));


			//interpolate between values
			float lowerCells = glm::mix(lowerLeftFunctionValue, lowerRightFunctionValue, interpolatorX);
			float upperCells = glm::mix(upperLeftFunctionValue, upperRightFunctionValue, interpolatorX);

			noiseAcc += scale;
			noise += glm::clamp(glm::mix(lowerCells, upperCells, interpolatorY) + 0.5f, 0.00001f, 0.99999f) * scale;
			scale *= 0.5f;
		}
		
		return glm::smoothstep(0.f, 1.f, noise / noiseAcc);
	}
}