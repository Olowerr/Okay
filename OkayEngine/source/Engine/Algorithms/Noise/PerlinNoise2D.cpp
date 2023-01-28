#include "PerlinNoise2D.h"
#include "Engine/Okay/OkayMath.h"

#include "imgui/imgui.h"


namespace Okay
{
	PerlinNoise2D::PerlinNoise2D(uint32_t seed)
		:seed(seed), octaves(8u), sections(INVALID_UINT), startOctWidth(512u), 
		bias(2.f), frequency(1.f), guiLockFreqRatio(true), dist(-1.f, 1.f)
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

		for (uint32_t x = 0; x < width; x++)
		{
			for (uint32_t y = 0; y < height; y++)
			{
				result[width * y + x] = UNORM_TO_UCHAR(sample((float)x, (float)y));
			}
		}

		DX11::updateTexture(resultBuffer, result, 1u, width, height);
		DX11::getInstance().getDeviceContext()->CopyResource(output, resultBuffer);

		DX11_RELEASE(resultBuffer);
		OKAY_DELETE_ARRAY(result);
	}

	float PerlinNoise2D::sample(float x, float y)
	{
		float noise = 0;
		float scale = 1.f;
		float scaleAcc = 0.f;

		// The unmodified version of gradient noise requires (for this project) low frequency
		x *= frequency.x * 0.002f;
		y *= frequency.y * 0.002f;

		for (uint32_t o = 0; o < octaves; o++)
		{
#if 0 // Value Noise - Based on OneLoneCoder/Javidx9s example
			int pitch = startOctWidth >> o;
			if (!pitch) pitch = 1u;

			const int intx = (int)x;
			const int inty = (int)y;

			// Works but remove ternary operator
			const int sampleX1 = intx < 0 ? intx - (pitch + intx % pitch) : (intx / pitch) * pitch;
			const int sampleY1 = inty < 0 ? inty - (pitch + inty % pitch) : (inty / pitch) * pitch;

			const int sampleX2 = (sampleX1 + pitch);
			const int sampleY2 = (sampleY1 + pitch);

			const float lerpTX = (x - (float)sampleX1) / (float)pitch;
			const float lerpTY = (y - (float)sampleY1) / (float)pitch;

			const float blendX1 = glm::mix(sampleSeed(sampleX1, sampleY1), sampleSeed(sampleX2, sampleY1), lerpTX);
			const float blendX2 = glm::mix(sampleSeed(sampleX1, sampleY2), sampleSeed(sampleX2, sampleY2), lerpTX);

			scaleAcc += scale;
			noise += glm::mix(blendX1, blendX2, lerpTY) * scale;
			scale /= bias;

#else // Gradient Noise - Based on the wiki, ronja-tutorials and Javidx9

			// Offset based on octave and half double the frequency
			const float oX = (x + o * 100.f) * (1 << o);
			const float oY = (y + o * 100.f) * (1 << o);

			const int sampleX1 = (int)glm::floor(oX);
			const int sampleX2 = sampleX1 + 1;
			const int sampleY1 = (int)glm::floor(oY);
			const int sampleY2 = sampleY1 + 1;

			// The position relative to the "octave square" | 12.0 - - 0.2 - - - - - - 13.0
			const glm::vec2 relPos(glm::fract(oX), glm::fract(oY));

			const float blendX1 = glm::dot(sampleSeed2(sampleX1, sampleY1), relPos - glm::vec2(0.f, 0.f));
			const float blendX2 = glm::dot(sampleSeed2(sampleX2, sampleY1), relPos - glm::vec2(1.f, 0.f));
			const float blendX3 = glm::dot(sampleSeed2(sampleX1, sampleY2), relPos - glm::vec2(0.f, 1.f));
			const float blendX4 = glm::dot(sampleSeed2(sampleX2, sampleY2), relPos - glm::vec2(1.f, 1.f));

			const float res1 = glm::mix(blendX1, blendX2, relPos.x);
			const float res2 = glm::mix(blendX3, blendX4, relPos.x);

			noise += glm::mix(res1, res2, relPos.y) * scale;
			scaleAcc += scale;
			scale /= bias;
#endif
		}

		//return sections == Okay::INVALID_UINT ? (noise / scaleAcc) * 0.5f + 0.5f : toon(noise / scaleAcc);
		//return sections == Okay::INVALID_UINT ? Okay::smoothStep((noise / scaleAcc) * 0.5f + 0.5f) : toon(noise / scaleAcc);
		return sections == Okay::INVALID_UINT ? glm::smoothstep(0.f, 1.f, (noise / scaleAcc) * 0.5f + 0.5f) : toon(noise / scaleAcc);

	}

	glm::vec2 PerlinNoise2D::sampleSeed2(int x, int y)
	{
		// Source: wikipedia, slightly modified :]

		static const uint32_t w = 32u;
		static const uint32_t s = w / 2;
		uint32_t a = x + seed * x, b = y + seed * y * 13;

		a *= 3284157443;
		b ^= a << s | a >> (w - s);
		b *= 1911520717;

		a ^= b << s | b >> (w - s);
		a *= 2048419325;

		const float random = a * (3.14159265f / ~(~0u >> 1));
		return glm::vec2(glm::cos(random), glm::sin(random));
	}

	float PerlinNoise2D::sampleSeed(int x, int y)
	{
		x += seed;

		// Source https://github.com/Cyan4973/xxHash

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
