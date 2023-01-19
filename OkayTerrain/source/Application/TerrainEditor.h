#pragma once
#include <Engine/Application/Application.h>
#include <Engine/Application/Scene.h>
#include <Engine/DirectX/DX11.h>
#include <Engine/Application/Entity.h>
#include <Engine/Graphics/Noise/PerlinNoise2D.h>
#include <Engine/Okay/Okay.h>

#include "imgui/imgui.h"

class TerrainEditor : public Application
{
public:
	
	TerrainEditor();
	~TerrainEditor();

	// Inherited via Application
	virtual void run() override;

private:
	Okay::Scene scene;
	Okay::PerlinNoise2D noiser;
	Okay::Entity water;
	Okay::Entity terrain;
	Okay::Entity obj;

	void update();
	void createTerrainMesh(uint32_t meshIdx = 0u)
	{
		createTerrainMesh(smoothShading, numSubDivs, scale, amplitude, meshIdx);
	}
	void createTerrainMesh(bool smoothShading, uint32_t subDivs, float scale, float amplitude, uint32_t meshIdx = 0u);


	bool smoothShading = false;
	int numSubDivs = 100;
	int numOct = 8;
	int numSec = 255;
	int octWidth = 512;
	int seed = 123;
	float bias = 2.f;
	float scale = 2048.f;
	float waterHeight = 0.f;
	bool lockOctWidth = true;
	bool wireFrame = false;
	glm::vec2 scroll = glm::vec2(0.f);
	glm::vec2 frequency = glm::vec2(1.f);
	float amplitude = 200.f;


	glm::vec3 findPos(glm::vec3 pos, uint32_t idx, uint32_t subDivs)
	{
		const float size = 1.f / (float)subDivs;
		const float xId = float(idx % subDivs);
		const float zId = float(idx / subDivs);

		pos.x += xId * size;
		pos.z -= zId * size;

		return pos;
	}
};