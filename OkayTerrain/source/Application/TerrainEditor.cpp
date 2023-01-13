#include "TerrainEditor.h"

#include <Engine/Components/Camera.h>
#include <Engine/Components/Transform.h>
#include <Engine/Components/MeshComponent.h>

#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

TerrainEditor::TerrainEditor()
	:Application(L"Okay Terrain"), scene(renderer)
{
	Application::initImgui();

	Okay::Entity camera = scene.createEntity();
	camera.addComponent<Okay::Camera>();
	camera.addComponent<Okay::PointLight>().intensity = 3.f;
	scene.setMainCamera(camera);

	content.addMesh();
	createTerrainMesh(10);

	Okay::Entity terrain = scene.createEntity();
	terrain.addComponent<Okay::MeshComponent>();
}

TerrainEditor::~TerrainEditor()
{
}

void TerrainEditor::run()
{
	using namespace Okay;

	DX11& dx11 = DX11::getInstance();
	Transform& tra = scene.getMainCamera().getComponent<Transform>();
	tra.rotation.x = glm::pi<float>() * 0.25f;

	scene.start();
	Time::start();

	while (window.isOpen())
	{
		Application::newFrameImGui();

		tra.rotation.y += Time::getDT();
		tra.calculateMatrix();
		tra.position = tra.forward() * -5.f;

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

}

void TerrainEditor::createTerrainMesh(uint32_t subDivs)
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

	for (uint32_t i = 0; i < subDivs * subDivs; i++)
	{
		for (size_t v = 0; v < NUM_VERTS; v++)
		{
			glm::vec3 pos = findPos(baseVerts[v], i, subDivs);
			data.positions.emplace_back(pos);

			data.uvs.emplace_back(pos.x + 0.5f, (pos.z - 0.5f) * -1.f);
			data.normals.emplace_back(0.f, 1.f, 0.f);

			data.indices.emplace_back((uint32_t)data.indices.size());
		}
	}

	content.getMesh(0u).create(data);
}
