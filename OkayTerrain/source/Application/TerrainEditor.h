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

	void update();
	void createTerrainMesh(uint32_t subDivs);

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