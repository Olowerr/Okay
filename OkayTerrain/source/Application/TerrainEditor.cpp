#include "TerrainEditor.h"

#include <Engine/Components/Camera.h>
#include <Engine/Components/Transform.h>
#include <Engine/Components/MeshComponent.h>

#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

TerrainEditor::TerrainEditor()
	:Application(L"Okay Terrain"), scene(renderer), noiser(8u, 255u, 2.f, 2048u)
{
	Application::initImgui();

	Okay::Entity camera = scene.createEntity();
	camera.addComponent<Okay::Camera>();
	camera.addComponent<Okay::PointLight>().intensity = 3.f;
	Okay::Transform& camTra = camera.getComponent<Okay::Transform>();
	camTra.rotation.x = glm::pi<float>() * 0.5f;
	camTra.position.y = 1000.f;

	scene.setMainCamera(camera);

	content.importFile("C:/Users/oliver/source/repos/Okay/OkayTerrain/resources/ground.png");
	content.importFile("C:/Users/oliver/source/repos/Okay/OkayTerrain/resources/water.png");

	content.addMesh();
	content.addMesh();

	noiser.setSeed(123);
	createTerrainMesh(200, 2048.f, 500.f);
	createTerrainMesh(1, 1.f, 0.f, 1u);


	content.addMaterial().setBaseColour(1u);
	terrain = scene.createEntity();
	terrain.addComponent<Okay::MeshComponent>(0u, 1u, 0u);

	content.addMaterial().setBaseColour(2u);
	water = scene.createEntity();
	water.addComponent<Okay::MeshComponent>(1u, 2u, 0u);
	water.getComponent<Okay::Transform>().position.y = 250.f;
}

TerrainEditor::~TerrainEditor()
{
}

void TerrainEditor::run()
{
	using namespace Okay;

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

void TerrainEditor::update()
{
	using namespace Okay;
	static bool open = true;

	ImGui::Begin("Settings", &open);
	ImGui::PushItemWidth(-100.f);

	static int numSubDivs = 200;
	static int numOct = 8;
	static int numSec = 255;
	static int octWidth = 2048;
	static int seed = 123;
	static float bias = 2.f;
	static float scale = 2048.f;
	static float scaleY = 500.f;
	static float camSpeed = 300.f;
	static float waterHeight = 250.f;
	static bool lockOctWidth = true;

	Transform& camTra = scene.getMainCamera().getComponent<Transform>();
	const float frameSpeed = camSpeed * Time::getDT();

	if (Input::isKeyDown(Keys::A))
		camTra.position.x -= frameSpeed;
	if (Input::isKeyDown(Keys::D))
		camTra.position.x += frameSpeed;
	if (Input::isKeyDown(Keys::W))
		camTra.position.z += frameSpeed;
	if (Input::isKeyDown(Keys::S))
		camTra.position.z -= frameSpeed;
	if (Input::isKeyDown(Keys::E))
		camTra.position.y += frameSpeed;
	if (Input::isKeyDown(Keys::Q))
		camTra.position.y -= frameSpeed;

	Transform& waTra = water.getComponent<Transform>();
	Transform& taTra = terrain.getComponent<Transform>();
	//taTra.position.x = scale * -0.5f;
	//taTra.position.z = scale * -0.5f;
	waTra.position.x = taTra.position.x;
	waTra.position.z = taTra.position.z;
	waTra.scale.x = scale;
	waTra.scale.z = scale;

	//ImGui::DragFloat("qweqd", &camTra.rotation.x, 0.1f);
	ImGui::DragFloat("cam speed", &camSpeed, 0.1f);
	ImGui::DragFloat("water height", &waTra.position.y, 0.1f);


	if (ImGui::InputInt("Seed", &seed, 1, 10))
	{
		noiser.setSeed(seed);
		createTerrainMesh(numSubDivs, scale, scaleY);
	}

	if (ImGui::InputInt("Num sections", &numSec, 1, 10))
	{
		numSec = glm::clamp(numSec, 1, 255);
		noiser.setSections(numSec);
		createTerrainMesh(numSubDivs, scale, scaleY);
	}

	if (ImGui::InputInt("Num octaves", &numOct, 1, 10))
	{
		numOct = glm::clamp(numOct, 0, 100);
		noiser.setOctaves(numOct);
		createTerrainMesh(numSubDivs, scale, scaleY);
	}

	
	if (ImGui::Checkbox("Lock Octave width", &lockOctWidth))
	{
		noiser.setOctWidth(octWidth);
		createTerrainMesh(numSubDivs, scale, scaleY);
	}

	if (lockOctWidth)
		octWidth = (int)scale;

	ImGui::BeginDisabled(lockOctWidth);
	if (ImGui::InputInt("octave width", &octWidth, 1, 10))
	{
		if (octWidth <= 0) octWidth = 1;

		noiser.setOctWidth(octWidth);
		createTerrainMesh(numSubDivs, scale, scaleY);
	}
	ImGui::EndDisabled();
	
	if (ImGui::DragFloat("Mesh scale", &scale, 1.f, 1.f))
	{
		createTerrainMesh(numSubDivs, scale, scaleY);
	}

	if (ImGui::DragFloat("Bias", &bias, 0.01f, 0.01f, 100.f, "%.4f"))
	{
		noiser.setBias(bias);
		createTerrainMesh(numSubDivs, scale, scaleY);
	}

	if (ImGui::InputInt("Sub Divs", &numSubDivs, 1, 10))
	{
		if (numSubDivs <= 0) numSubDivs = 1;
		createTerrainMesh(numSubDivs, scale, scaleY);
	}

	if (ImGui::DragFloat("Scale Y", &scaleY, 0.1f))
	{
		createTerrainMesh(numSubDivs, scale, scaleY);
	}


	if (ImGui::Button("New seed"))
	{
		//noiser->randomizeSeed();
		//noiser->generate(numOct, numSec, bias);
	}

	ImGui::PopItemWidth();
	ImGui::End();
}

void TerrainEditor::createTerrainMesh(uint32_t subDivs, float scale, float scaleY, uint32_t meshIdx)
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

	for (uint32_t i = 0; i < subDivs * subDivs; i++)
	{
		for (size_t v = 0; v < NUM_VERTS; v++)
		{
			glm::vec3 pos = findPos(baseVerts[v], i, subDivs);

			pos *= scale;
#if 0
			pos.y += noiser.sample((int)pos.x, (int)pos.z) * scaleY;
#else
			float sampleX1 = noiser.sample((int)pos.x, (int)pos.z) * scaleY;
			float sampleX2 = noiser.sample((int)pos.x + 1, (int)pos.z + 1) * scaleY;
			float lerpT = std::abs(std::fmod(pos.x, 1.f));

			pos.y += glm::mix(sampleX1, sampleX2, lerpT);
#endif

			data.positions.emplace_back(pos);

			data.uvs.emplace_back(pos.x + 0.5f, (pos.z - 0.5f) * -1.f);

			data.indices.emplace_back((uint32_t)data.indices.size());
		}
	}

	for (size_t i = 0; i < numPoints; i+=3)
	{
		glm::vec3 v1, v2, result;
		v1 = data.positions[i + 1] - data.positions[i];
		v2 = data.positions[i + 2] - data.positions[i];

		result = glm::normalize(glm::cross(v1, v2));
		// hmm

		data.normals.emplace_back(result);
		data.normals.emplace_back(result);
		data.normals.emplace_back(result);
	}

	content.getMesh(meshIdx).create(data);
}
