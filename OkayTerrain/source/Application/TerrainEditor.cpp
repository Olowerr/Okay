#include "TerrainEditor.h"

#include "../Movement.h"

#include <Engine/Components/Camera.h>
#include <Engine/Components/Transform.h>
#include <Engine/Components/MeshComponent.h>
#include <Engine/Application/Script/DefaultScripts/FreeLookMovement.h>

#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

TerrainEditor::TerrainEditor()
	:Application(L"Okay Terrain"), scene(renderer), noiser(8u)
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
	water.getComponent<Okay::Transform>().position.y = waterHeight;
	

	content.importFile("C:/Users/oliver/source/repos/Okay/OkayTerrain/resources/ground.png");
	content.importFile("C:/Users/oliver/source/repos/Okay/OkayTerrain/resources/water.png");

	content.addMesh();
	content.addMesh();

	noiser.setSeed(123);
	createTerrainMesh();
	createTerrainMesh(false, 1, 1.f, 0.f, 1u);

	Okay::Transform& waTra = water.getComponent<Okay::Transform>();
	const Okay::Transform& taTra = terrain.getComponent<Okay::Transform>();
	waTra.position.x = taTra.position.x;
	waTra.position.z = taTra.position.z;
	waTra.scale.x = scale;
	waTra.scale.z = scale;
	
	content.importFile("C:/Users/Oliver/source/repos/Okay/OkayEditor/resources/Meshes/gob.obj");
	obj = scene.createEntity();
	obj.addComponent<Okay::MeshComponent>(2u);
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

		scene.update();
		update();

		scene.submit();
		renderer.render(scene.getMainCamera());

		Application::endFrameImGui();
	}

	Application::destroyImgui();
}

void TerrainEditor::update()
{
	using namespace Okay;
	static bool open = true;

	ImGui::Begin("Settings", &open);

	static float camSpeed = scene.getMainCamera().getScript<FreeLookMovement>().getSpeed();
	static bool ctrlPlayer = false;

	Transform& waTra = water.getComponent<Transform>();
	Transform& plaTra = obj.getComponent<Transform>();

	ImGui::PushItemWidth(-20.f);
	ImGui::Text("Player");
	if (ImGui::Checkbox("Control", &ctrlPlayer))
	{
		if (ctrlPlayer)
		{
			Okay::Entity cam = scene.getMainCamera();
			cam.removeScript<Okay::FreeLookMovement>();
			obj.addScript<ThirdPersonMovement>(cam);
		}
		else
		{
			obj.removeScript<ThirdPersonMovement>();
			scene.getMainCamera().addScript<Okay::FreeLookMovement>();
		}
	}

	ImGui::BeginDisabled(ctrlPlayer);
	ImGui::Text("Position:"); ImGui::SameLine();
	ImGui::DragFloat3("##TraposNL", &plaTra.position.x, 0.01f);
	ImGui::EndDisabled();

	ImGui::Text("Scale:   "); ImGui::SameLine();
	ImGui::DragFloat("##TrascaNL", &plaTra.scale.x, 0.01f);
	plaTra.scale.y = plaTra.scale.z = plaTra.scale.x;

	ImGui::Separator();
	ImGui::PopItemWidth();

	ImGui::PushItemWidth(-100.f);

	if (ctrlPlayer)
	{
		plaTra.position.y = noiser.sample(plaTra.position.x * frequency.x + scroll.x, plaTra.position.z * frequency.y + scroll.y);
		plaTra.position.y = std::pow(plaTra.position.y, exponent) * 2.f - 1.f;
		plaTra.position.y *= amplitude;
	}

	static bool lockFreq = true;
	if (ImGui::Checkbox("Lock Y Frequency", &lockFreq))
	{
		if (lockFreq)
			frequency.y = frequency.x;

		createTerrainMesh();
	}

	if (ImGui::DragFloat2("Frequency", &frequency.x, 0.005f))
	{
		if (lockFreq)
			frequency.y = frequency.x;

		createTerrainMesh();
	}
	
	if (ImGui::DragFloat("Amplitude", &amplitude, 0.1f))
		createTerrainMesh();
	

	if (ImGui::DragFloat2("Scroll", &scroll.x, 1.f))
		createTerrainMesh();

	if (ImGui::Checkbox("Wireframe", &wireFrame))
		renderer.setWireframe(wireFrame);

	if (ImGui::Checkbox("Smooth shading (slower)", &smoothShading))
		createTerrainMesh();

	if (ImGui::DragFloat("cam speed", &camSpeed, 0.1f))
	{
		FreeLookMovement& movement = scene.getMainCamera().getScript<FreeLookMovement>();
		movement.setSpeed(camSpeed);
	}

	if (ImGui::DragFloat("Expo", &exponent, 0.01f))
	{
		createTerrainMesh();
	}

	ImGui::DragFloat("water height", &waTra.position.y, 0.1f);

	if (ImGui::InputInt("Seed", &seed, 1, 10))
	{
		noiser.setSeed(seed);
		createTerrainMesh();
	}

	static bool useSections = false;
	if (ImGui::Checkbox("Use sections", &useSections))
	{
		if (!useSections)
			noiser.setSections(Okay::INVALID_UINT);
	}
	ImGui::BeginDisabled(!useSections);
	if (ImGui::InputInt("Num sections", &numSec, 1, 10))
	{
		numSec = glm::clamp(numSec, 1, 255);
		noiser.setSections(numSec);
		createTerrainMesh();
	}
	ImGui::EndDisabled();

	if (ImGui::InputInt("Num octaves", &numOct, 1, 10))
	{
		numOct = glm::clamp(numOct, 0, 100);
		noiser.setOctaves(numOct);
		createTerrainMesh();
	}

	
	if (ImGui::Checkbox("Lock Octave width", &lockOctWidth))
	{
		noiser.setOctWidth(octWidth);
		createTerrainMesh();
	}

	if (lockOctWidth)
		octWidth = (int)scale;

	ImGui::BeginDisabled(lockOctWidth);
	if (ImGui::InputInt("octave width", &octWidth, 1, 10))
	{
		if (octWidth <= 0) octWidth = 1;

		noiser.setOctWidth(octWidth);
		createTerrainMesh();
	}
	ImGui::EndDisabled();
	
	if (ImGui::DragFloat("Mesh scale", &scale, 1.f, 1.f))
	{
		createTerrainMesh();
	}

	if (ImGui::DragFloat("Bias", &bias, 0.01f, 0.01f, 100.f, "%.4f"))
	{
		noiser.setBias(bias);
		createTerrainMesh();
	}

	if (ImGui::InputInt("Sub Divs", &numSubDivs, 1, 10))
	{
		if (numSubDivs <= 0) numSubDivs = 1;
		createTerrainMesh();
	}

	ImGui::PopItemWidth();
	ImGui::End();
}

void TerrainEditor::createTerrainMesh(bool smoothShading, uint32_t subDivs, float scale, float amplitude, uint32_t meshIdx)
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
			glm::vec3 pos = findPos(baseVerts[v], i, subDivs) * scale;
			
			float noise = noiser.sample(pos.x * frequency.x + scroll.x, pos.z * frequency.y + scroll.y);
			noise = std::pow(noise, exponent);
			pos.y += noise * 2.f - 1.f;
			pos.y *= amplitude;

			if (pos.y < minY)
				minY = pos.y;

			data.positions.emplace_back(pos);
			data.uvs.emplace_back(pos.x / scale + 0.5f, pos.z / -scale + 0.5f);
			data.indices.emplace_back((uint32_t)data.indices.size());
		}
	}

	//for (size_t i = 0; i < numPoints; i++)
	//	data.positions[i].y -= minY;
	

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

		auto searchQuads = [&](int startIdx, size_t curIdx, int numQuads, glm::vec3& sum)
		{
			if (startIdx < 0 || startIdx >= numPoints / 3)
				return;

			for (int j = 0; j < NUM_VERTS * 10; j++)
			{
				int vert = startIdx * 3 + j;
				if (vert < 0 || vert >= numPoints || vert == curIdx)
					continue;

				if (glm::vec3 delta = data.positions[curIdx] - data.positions[vert]; glm::dot(delta, delta) > 0.5f * 0.5f)
					continue;

				sum += faceNormals[vert / 3];
			}
		};

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

			int idx1 = (int)faceIdx - (subDivs * 2 - 1) - 4;
			searchQuads(idx1, i, 6, sum);
			
			int idx2 = (int)faceIdx - 4;
			searchQuads(idx2, i, 6, sum);
			
			int idx3 = (int)faceIdx + (subDivs * 2) - 4;
			searchQuads(idx3, i, 6, sum);
			

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
