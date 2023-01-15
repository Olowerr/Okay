#include "TerrainEditor.h"

#include <Engine/Components/Camera.h>
#include <Engine/Components/Transform.h>
#include <Engine/Components/MeshComponent.h>
#include <Engine/Application/Script/DefaultScripts/FreeLookMovement.h>

#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

TerrainEditor::TerrainEditor()
	:Application(L"Okay Terrain"), scene(renderer), noiser(8u, 255u, 2.f, 512u)
{
	Okay::Entity camera = scene.createEntity();
	camera.addComponent<Okay::Camera>();
	camera.addComponent<Okay::PointLight>().intensity = 3.f;
	camera.addScript<Okay::FreeLookMovement>(500.f);
	camera.getComponent<Okay::Transform>().position.y = 300.f;
	scene.setMainCamera(camera);

	content.addMaterial().setBaseColour(1u);
	terrain = scene.createEntity();
	terrain.addComponent<Okay::MeshComponent>(0u, 1u, 0u);

	content.addMaterial().setBaseColour(2u);
	water = scene.createEntity();
	water.addComponent<Okay::MeshComponent>(1u, 2u, 0u);
	water.getComponent<Okay::Transform>().position.y = 250.f;

	content.importFile("C:/Users/oliver/source/repos/Okay/OkayTerrain/resources/ground.png");
	content.importFile("C:/Users/oliver/source/repos/Okay/OkayTerrain/resources/water.png");

	content.addMesh();
	content.addMesh();

	noiser.setSeed(123);
	createTerrainMesh(false, 100, 2048.f, 500.f);
	createTerrainMesh(false, 1, 1.f, 0.f, 1u);


	Okay::Transform& waTra = water.getComponent<Okay::Transform>();
	const Okay::Transform& taTra = terrain.getComponent<Okay::Transform>();
	waTra.position.x = taTra.position.x;
	waTra.position.z = taTra.position.z;
	waTra.scale.x = 2048.f;
	waTra.scale.z = 2048.f;
}

TerrainEditor::~TerrainEditor()
{
}

void TerrainEditor::run()
{
	using namespace Okay;
	Application::initImgui();

	scene.start();
	Time::start();

	while (window.isOpen())
	{
		Application::newFrameImGui();

		update();
		scene.update();

		scene.submit();
		renderer.render(scene.getMainCamera());

		Application::endFrameImGui();
	}

	Application::destroyImgui();
}

#define CREATE_TERRAIN() createTerrainMesh(smoothShading, numSubDivs, scale, scaleY)
void TerrainEditor::update()
{
	using namespace Okay;
	static bool open = true;

	ImGui::Begin("Settings", &open);
	ImGui::PushItemWidth(-100.f);

	static bool smoothShading = false;
	static int numSubDivs = 100;
	static int numOct = 8;
	static int numSec = 255;
	static int octWidth = 512;
	static int seed = 123;
	static float bias = 2.f;
	static float scale = 2048.f;
	static float scaleY = 500.f;
	static float camSpeed = scene.getMainCamera().getScript<FreeLookMovement>().getSpeed();
	static float waterHeight = 250.f;
	static bool lockOctWidth = true;

	Transform& waTra = water.getComponent<Transform>();

	if (ImGui::Checkbox("Smooth shading (slow)", &smoothShading))
	{
		CREATE_TERRAIN();
	}
	if (ImGui::DragFloat("cam speed", &camSpeed, 0.1f))
	{
		FreeLookMovement& movement = scene.getMainCamera().getScript<FreeLookMovement>();
		movement.setSpeed(camSpeed);
	}

	ImGui::DragFloat("water height", &waTra.position.y, 0.1f);

	if (ImGui::InputInt("Seed", &seed, 1, 10))
	{
		noiser.setSeed(seed);
		CREATE_TERRAIN();
	}

	if (ImGui::InputInt("Num sections", &numSec, 1, 10))
	{
		numSec = glm::clamp(numSec, 1, 255);
		noiser.setSections(numSec);
		CREATE_TERRAIN();
	}

	if (ImGui::InputInt("Num octaves", &numOct, 1, 10))
	{
		numOct = glm::clamp(numOct, 0, 100);
		noiser.setOctaves(numOct);
		CREATE_TERRAIN();
	}

	
	//if (ImGui::Checkbox("Lock Octave width", &lockOctWidth))
	//{
	//	noiser.setOctWidth(octWidth);
	//	createTerrainMesh(numSubDivs, scale, scaleY);
	//}

	//if (lockOctWidth)
	//	octWidth = (int)scale;

	//ImGui::BeginDisabled(lockOctWidth);
	if (ImGui::InputInt("octave width", &octWidth, 1, 10))
	{
		if (octWidth <= 0) octWidth = 1;

		noiser.setOctWidth(octWidth);
		CREATE_TERRAIN();
	}
	//ImGui::EndDisabled();
	
	if (ImGui::DragFloat("Mesh scale", &scale, 1.f, 1.f))
	{
		CREATE_TERRAIN();
	}

	if (ImGui::DragFloat("Bias", &bias, 0.01f, 0.01f, 100.f, "%.4f"))
	{
		noiser.setBias(bias);
		CREATE_TERRAIN();
	}

	if (ImGui::InputInt("Sub Divs", &numSubDivs, 1, 10))
	{
		if (numSubDivs <= 0) numSubDivs = 1;
		CREATE_TERRAIN();
	}

	if (ImGui::DragFloat("Scale Y", &scaleY, 0.1f))
	{
		CREATE_TERRAIN();
	}

	ImGui::PopItemWidth();
	ImGui::End();
}

void TerrainEditor::createTerrainMesh(bool smoothShading, uint32_t subDivs, float scale, float scaleY, uint32_t meshIdx)
{
	using namespace Okay;

	Mesh::MeshInfo data;

	glm::vec3 baseVerts[] = 
	{
		{ -0.5f,  0.f,  0.5f},
		{  0.5f,  0.f,  0.5f},
		{ -0.5f,  0.f, -0.5f},

		{ -0.5f,  0.f, -0.5f},
		{  0.5f,  0.f,  0.5f},
		{  0.5f,  0.f, -0.5f},
	};

	const size_t NUM_VERTS = ARRAY_SIZE(baseVerts);
	if (!subDivs) subDivs = 1;

	const float size = 1.f / (float)subDivs;
	for (size_t i = 0; i < NUM_VERTS; i++)
	{
		baseVerts[i] *= size;
		baseVerts[i].x -= 0.5f - std::abs(baseVerts[i].x);
		baseVerts[i].z += 0.5f - std::abs(baseVerts[i].z);
	}

	size_t numPoints = (size_t)subDivs * (size_t)subDivs * NUM_VERTS;

	data.positions.reserve(numPoints);
	data.uvs.reserve(numPoints);
	data.normals.reserve(numPoints);
	data.indices.reserve(numPoints);

	float minY = 1000000000000.f;
	for (uint32_t i = 0; i < subDivs * subDivs; i++)
	{
		for (size_t v = 0; v < NUM_VERTS; v++)
		{
			glm::vec3 pos = findPos(baseVerts[v], i, subDivs);

			pos *= scale;
#if 1
			pos.y += noiser.sample((int)pos.x, (int)pos.z) * scaleY;
#else
			float sampleX1 = noiser.sample((int)pos.x, (int)pos.z) * scaleY;
			float sampleX2 = noiser.sample((int)pos.x + 1, (int)pos.z + 1) * scaleY;
			float lerpT = std::abs(std::fmod(pos.x, 1.f));

			pos.y += glm::mix(sampleX1, sampleX2, lerpT);
#endif

			if (pos.y < minY)
				minY = pos.y;

			data.positions.emplace_back(pos);

			data.uvs.emplace_back(pos.x / scale, pos.z / scale);

			data.indices.emplace_back((uint32_t)data.indices.size());
		}
	}

	for (size_t i = 0; i < numPoints; i++)
		data.positions[i].y -= minY;
	

	if (smoothShading)
	{

		std::vector<glm::vec3> faceNormals(numPoints / 3);
		size_t counter = 0;
		for (size_t i = 0; i < numPoints; i += 3)
		{
			glm::vec3 v1, v2, result;
			v1 = data.positions[i + 1] - data.positions[i];
			v2 = data.positions[i + 2] - data.positions[i];

			result = glm::cross(v1, v2);

			faceNormals[counter++] = result;
		}

		glm::vec3 sum(0.f);
		for (size_t i = 0; i < numPoints; i++)
		{
			const size_t faceIdx = i / 3;
			sum = faceNormals[faceIdx];

			if (i == 1202)
			{
				int q = 0;
			}

			/*int span = (int)subDivs * 7;
			int startIdx = (int)i - span;

			for (int j = startIdx; j < i + span; j++)
			{
				if (j < 0 || j >= numPoints || j == i)
					continue;

				if (glm::vec3 delta = data.positions[i] - data.positions[j]; glm::dot(delta, delta) > 0.5f * 0.5f)
					continue;

				sum += faceNormals[j / 3];
			}*/


			{
				int idx = (int)faceIdx - (subDivs * 2 - 1) - 4;
				if (idx >= 0 && idx < numPoints / 3)
				{
					for (int j = 0; j < NUM_VERTS * 10; j++)
					{
						int vert = idx * 3 + j;
						if (vert < 0 || vert >= numPoints || vert == i)
							continue;

						if (glm::vec3 delta = data.positions[i] - data.positions[vert]; glm::dot(delta, delta) > 0.5f * 0.5f)
							continue;

						sum += faceNormals[vert / 3];
					}
				}
			}
			
			{
				int idx = (int)faceIdx - 4;
				if (idx >= 0 && idx < numPoints / 3)
				{
					for (int j = 0; j < NUM_VERTS * 10; j++)
					{
						int vert = idx * 3 + j;
						if (vert < 0 || vert >= numPoints || vert == i)
							continue;

						if (glm::vec3 delta = data.positions[i] - data.positions[vert]; glm::dot(delta, delta) > 0.5f * 0.5f)
							continue;

						sum += faceNormals[vert / 3];
					}
				}
			}
			
			{
				int idx = (int)faceIdx + (subDivs * 2) - 4;
				if (idx >= 0 && idx < numPoints / 3)
				{
					for (int j = 0; j < NUM_VERTS * 10; j++)
					{
						int vert = idx * 3 + j;
						if (vert < 0 || vert >= numPoints || vert == i)
							continue;

						if (glm::vec3 delta = data.positions[i] - data.positions[vert]; glm::dot(delta, delta) > 0.5f * 0.5f)
							continue;

						sum += faceNormals[vert / 3];
					}
				}
			}

			/*for (size_t j = 0; j < numPoints; j++)
			{
				if (i == j)
					continue;

				if (glm::vec3 delta = data.positions[i] - data.positions[j]; glm::dot(delta, delta) > 0.5f * 0.5f)
					continue;

				sum += faceNormals[j / 3];
			}*/
			data.normals.emplace_back(glm::normalize(sum));
		}
	}
	else
	{
		for (size_t i = 0; i < numPoints; i += 3)
		{
			glm::vec3 v1, v2, result;
			v1 = data.positions[i + 1] - data.positions[i];
			v2 = data.positions[i + 2] - data.positions[i];

			result = glm::normalize(glm::cross(v1, v2));

			data.normals.emplace_back(result);
			data.normals.emplace_back(result);
			data.normals.emplace_back(result);
		}
	}

	content.getMesh(meshIdx).create(data);

	Transform& waTra = water.getComponent<Transform>();
	const Transform& taTra = terrain.getComponent<Transform>();
	waTra.position.x = taTra.position.x;
	waTra.position.z = taTra.position.z;
	waTra.scale.x = scale;
	waTra.scale.z = scale;
}
