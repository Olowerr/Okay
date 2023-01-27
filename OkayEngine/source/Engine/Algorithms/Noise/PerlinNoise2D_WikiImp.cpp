#include "PerlinNoise2D.h"

namespace Okay
{

	glm::vec2 randomGradient(int ix, int iy) {
		const unsigned w = 8 * sizeof(unsigned);
		const unsigned s = w / 2;
		unsigned a = ix, b = iy;
		a *= 3284157443; b ^= a << s | a >> w - s;
		b *= 1911520717; a ^= b << s | b >> w - s;
		a *= 2048419325;
		float random = a * (3.14159265f / ~(~0u >> 1));
		glm::vec2 v;
		v.x = cos(random); v.y = sin(random);
		return v;
	}


	float dotGridGradient(int ix, int iy, float x, float y) {
		glm::vec2 gradient = randomGradient(ix, iy);

		float dx = x - (float)ix;
		float dy = y - (float)iy;

		// Compute the dot-product
		return (dx * gradient.x + dy * gradient.y);

	}

	float PerlinNoise2D::sample2(float x, float y)
	{
		x *= frequency.x * 0.01f;
		y *= frequency.y * 0.01f;

		// temp fix
		x = std::abs(x);
		y = std::abs(y);

#if 1
		const int sampleX1 = (int)x;
		const int sampleY1 = (int)y;

		const int sampleX2 = sampleX1 + 1;
		const int sampleY2 = sampleY1 + 1;

		const float lerpTX = glm::fract(x);
		const float lerpTY = glm::fract(y);
#else
		// Multiply gradient funcs with startOctWidth ?

		const int intx = (int)x;
		const int inty = (int)y;

		const int sampleX1 = (intx / startOctWidth) * startOctWidth;
		const int sampleY1 = (inty / startOctWidth) * startOctWidth;

		const int sampleX2 = sampleX1 + startOctWidth;
		const int sampleY2 = sampleY1 + startOctWidth;

		const float lerpTX = (x - (float)sampleX1) / (float)startOctWidth;
		const float lerpTY = (y - (float)sampleY1) / (float)startOctWidth;
#endif

		const float blendX1 = dotGridGradient(sampleX1, sampleY1, x, y);
		const float blendX2 = dotGridGradient(sampleX2, sampleY1, x, y);
		const float res1 = glm::mix(blendX1, blendX2, lerpTX);

		const float blendX3 = dotGridGradient(sampleX1, sampleY2, x, y);
		const float blendX4 = dotGridGradient(sampleX2, sampleY2, x, y);
		const float res2 = glm::mix(blendX3, blendX4, lerpTX);

		//return smoothStep(glm::mix(res1, res2, lerpTY) * 0.5f + 0.5f);
		//return smoothstep(0.f, 1.f, glm::mix(res1, res2, lerpTY) * 0.5f + 0.5f);
		return glm::mix(res1, res2, lerpTY) * 0.5f + 0.5f;
	}
}