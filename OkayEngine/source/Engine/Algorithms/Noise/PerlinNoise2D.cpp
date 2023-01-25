#include "PerlinNoise2D.h"
#include "imgui/imgui.h"

namespace Okay
{
	PerlinNoise2D::PerlinNoise2D(uint32_t seed)
		:seed(seed), octaves(8u), sections(INVALID_UINT), startOctWidth(512u), 
		bias(2.f), frequency(1.f), guiLockFreqRatio(true)
	{
	}

	PerlinNoise2D::~PerlinNoise2D()
	{
	}

	void PerlinNoise2D::generateTexture(ID3D11Texture2D* output)
	{
		uint32_t width = 0u, height = 0u;
		ID3D11Texture2D* resultBuffer = nullptr;
		createResources(output, &resultBuffer, &width, &height);

		unsigned char* result = new unsigned char[(size_t)width * (size_t)height]{};

		float min = 100000000.f;
		float max = -100000000.f;

		for (uint32_t x = 0; x < width; x++)
		{
			for (uint32_t y = 0; y < height; y++)
			{
				float res = sample2((float)(x + 0.5f) * frequency.x, (float)(y + 0.5f) * frequency.y);
				if (res < min) min = res;
				if (res > max) max = res;

				result[width * y + x] = UNORM_TO_UCHAR(res);
			}
		}

		printf("Min: %.7f, Max: %.7f\n", min, max);

		DX11::updateTexture(resultBuffer, result, 1u, width, height);
		DX11::getInstance().getDeviceContext()->CopyResource(output, resultBuffer);

		DX11_RELEASE(resultBuffer);
		OKAY_DELETE_ARRAY(result);
	}
	
	float hash(int value)
	{
		/* mix around the bits in x: */
		value = value * 3266489917 + 374761393;
		value = (value << 17) | (value >> 15);

		/* Give value a good stir: */
		value *= 668265263;
		value ^= value >> 15;
		value *= 2246822519;
		value ^= value >> 13;
		value *= 3266489917;
		value ^= value >> 16;

		/* trim the result and scale it to a float in [0,1): */
		return (value & 0x00ffffff) * (1.0f / 0x1000000);
	}

	glm::vec2 randomGradient2(int ix, int iy) {
		// No precomputed gradients mean this works for any number of grid coordinates
		const unsigned w = 8 * sizeof(unsigned);
		const unsigned s = w / 2; // rotation width
		unsigned a = ix, b = iy;
		a *= 3284157443; b ^= a << s | a >> w - s;
		b *= 1911520717; a ^= b << s | b >> w - s;
		a *= 2048419325;
		float random = a * (3.14159265f / ~(~0u >> 1)); // in [0, 2*Pi]
		glm::vec2 v;
		v.x = cos(random); v.y = sin(random);
		return v;
	}

	glm::vec2 randomGradient(int x, int y) 
	{
		return glm::vec2(hash(x), hash(y));
	}

	// Computes the dot product of the distance and gradient vectors.
	float dotGridGradient(int ix, int iy, float x, float y) {
		// Get gradient from integer coordinates
		glm::vec2 gradient = randomGradient2(ix, iy);

		// Compute the distance vector
		float dx = x - (float)ix;
		float dy = y - (float)iy;

		// Compute the dot-product
		return (dx * gradient.x + dy * gradient.y);
	}

	float PerlinNoise2D::sample(float x, float y)
	{
		float noise = 0;
		float scale = 1.f;
		float scaleAcc = 0.f;

		x *= frequency.x;
		y *= frequency.y;

		for (uint32_t o = 0; o < octaves; o++)
		{
			//float oX = x * (o + 1);
			//float oY = y * (o + 1);
			//float oX = x * std::pow(2.f, (float)o);
			//float oY = y * std::pow(2.f, (float)o);
			float oX = x;
			float oY = y;

#if 1 // My version
			int pitch = startOctWidth >> o;
			if (!pitch) pitch = 1u;

			const int intx = (int)oX;
			const int inty = (int)oY;

			// Gave incorrect results with negative inputs
			//const int sampleX1 = (x / pitchX) * pitchX;
			//const int sampleY1 = (y / pitchY) * pitchY;
			
			// Works but remove ternary operator
			const int sampleX1 = intx < 0 ? intx - (pitch + intx % pitch) : (intx / pitch) * pitch;
			const int sampleY1 = inty < 0 ? inty - (pitch + inty % pitch) : (inty / pitch) * pitch;

			const int sampleX2 = (sampleX1 + pitch);
			const int sampleY2 = (sampleY1 + pitch);

			// Gave incorrect results with negative inputs
			//const float lerpTX = (float)(x % pitchX) / (float)pitchX;
			//const float lerpTY = (float)(y % pitchY) / (float)pitchY;

			// OneLoneCoder/Javidx9's blend math
			const float lerpTX = (oX - (float)sampleX1) / (float)pitch;
			const float lerpTY = (oY - (float)sampleY1) / (float)pitch;

			const float blendX1 = glm::mix(sampleSeed(sampleX1, sampleY1), sampleSeed(sampleX2, sampleY1), lerpTX);
			const float blendX2 = glm::mix(sampleSeed(sampleX1, sampleY2), sampleSeed(sampleX2, sampleY2), lerpTX);

			scaleAcc += scale;
			noise += glm::mix(blendX1, blendX2, lerpTY) * scale;
			scale /= bias;

#else // OneLoneCoder/Javidx9's version
			int nPitch = startWidth >> o;
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

		return sections == Okay::INVALID_UINT ? noise / scaleAcc : toon(noise / scaleAcc);

	}

	float PerlinNoise2D::sample2(float x, float y)
	{
		x *= frequency.x * 0.01f;
		y *= frequency.y * 0.01f;

		x = std::abs(x);
		y = std::abs(y);

#if 0
		const int sampleX1 = (int)x;
		const int sampleY1 = (int)y;

		const int sampleX2 = sampleX1 + 1;
		const int sampleY2 = psampleY1 + 1;

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

		return glm::mix(res1, res2, lerpTY) * 0.5f + 0.5f;
	}

	float PerlinNoise2D::sampleSeed(int x, int y)
	{
		x += seed;

		// https://github.com/Cyan4973/xxHash

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

	void PerlinNoise2D::createResources(ID3D11Texture2D* output, ID3D11Texture2D** resultBuffer, uint32_t* width, uint32_t* height)
	{
		D3D11_TEXTURE2D_DESC texDesc{};
		output->GetDesc(&texDesc);
		*width = texDesc.Width;
		*height = texDesc.Height;

		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		texDesc.Format = DXGI_FORMAT_R8_UNORM;
		texDesc.ArraySize = 1u;
		texDesc.MipLevels = 1u;
		texDesc.SampleDesc.Count = 1u;
		texDesc.SampleDesc.Quality = 0u;
		texDesc.MiscFlags = 0u;
		texDesc.Usage = D3D11_USAGE_DEFAULT;

		DX11::getInstance().getDevice()->CreateTexture2D(&texDesc, nullptr, resultBuffer);
	}

	bool PerlinNoise2D::imgui(const char* label)
	{
		if (!ImGui::Begin(label))
		{
			ImGui::End();
			return false;
		}
		bool pressed = false;
		
		ImGui::PushItemWidth(-1.f);

		ImGui::Text("Seed:");
		if (ImGui::InputInt("##NLSeed", (int*)&seed))
		{
			seed = seed == Okay::INVALID_UINT ? 0u : seed;
			pressed = true;
		}

		ImGui::Text("Octaves:");
		if (ImGui::InputInt("##NLOct", (int*)&octaves, 1, 0))
		{
			octaves = octaves == Okay::INVALID_UINT ? 0u : octaves;
			pressed = true;
		}

		ImGui::Text("Sections:");
		ImGui::SameLine();
		if (ImGui::Button("Toggle"))
		{
			sections = sections == Okay::INVALID_UINT ? 8u : Okay::INVALID_UINT;
			pressed = true;
		}
		ImGui::BeginDisabled(sections == Okay::INVALID_UINT);
		if (ImGui::InputInt("##NLSec", (int*)&sections))
			pressed = true;
		ImGui::EndDisabled();

		ImGui::Text("Octave width:");
		if (ImGui::InputInt("##NLOctW", (int*)&startOctWidth, 1, 0))
		{
			startOctWidth = octaves == Okay::INVALID_UINT ? 0u : startOctWidth;
			pressed = true;
		}

		ImGui::Text("Bias:");
		if (ImGui::DragFloat("##NLbias", &bias, 0.01f))
			pressed = true;

		ImGui::Text("Frequency:");
		if (ImGui::Checkbox("Lock Y frequency", &guiLockFreqRatio))
		{
			frequency.y = guiLockFreqRatio ? frequency.x : frequency.y;
			pressed = true;
		}
		if (ImGui::DragFloat2("##NLfreq", &frequency.x, 0.01f))
		{
			frequency.y = guiLockFreqRatio ? frequency.x : frequency.y;
			pressed = true;
		}


		ImGui::PopItemWidth();
		ImGui::End();
		return pressed;
	}
}
