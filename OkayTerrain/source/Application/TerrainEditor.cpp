#include "TerrainEditor.h"

#include "../Movement.h"

#include <Engine/Components/Camera.h>
#include <Engine/Components/Transform.h>
#include <Engine/Components/MeshComponent.h>
#include <Engine/Script/DefaultScripts/FreeLookMovement.h>

#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#include <execution>

TerrainEditor::TerrainEditor()
	:Application(L"Okay Terrain"), scene(renderer), noiser(123u)
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

#if 0
	lerpPoints.addPoint(0.0f, -0.1f);
	lerpPoints.addPoint(0.1f, -0.1f);
	lerpPoints.addPoint(0.2f, -0.1f);
	lerpPoints.addPoint(0.4f, -0.1f);
	lerpPoints.addPoint(0.6f, 0.33f);
	lerpPoints.addPoint(0.8f, 2.244f);
	lerpPoints.addPoint(1.0f, 3.640f);
#elif 1
	lerpPoints.addPoint(0.0f, 0.5f);
	lerpPoints.addPoint(0.42f, 0.08f);
	lerpPoints.addPoint(0.49f, -0.095f);
	lerpPoints.addPoint(0.51f, -0.146f);
	lerpPoints.addPoint(0.53f, 0.002f);
	lerpPoints.addPoint(0.56f, 0.091f);
	lerpPoints.addPoint(0.87f, 1.09f);
	lerpPoints.addPoint(1.f, 1.55f);
#else
	lerpPoints.addPoint(0.0f, 0.f);
	lerpPoints.addPoint(1.0f, 1.f);
#endif

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

	if (lerpPoints.imgui("Terrain height"))
		createTerrainMesh();

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
			scene.getMainCamera().addScript<Okay::FreeLookMovement>(camSpeed);
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
		plaTra.position.y = noiser.sample(plaTra.position.x + scroll.x, plaTra.position.z + scroll.y);
		plaTra.position.y = std::pow(plaTra.position.y, exponent);
		plaTra.position.y = lerpPoints.sample(plaTra.position.y);
		plaTra.position.y *= amplitude;

	}
	
	if (ImGui::DragFloat("Amplitude", &amplitude, 0.1f))
		createTerrainMesh();
	

	if (ImGui::DragFloat2("Scroll", &scroll.x, 1.f))
		createTerrainMesh();

	if (ImGui::Checkbox("Wireframe", &wireFrame))
		renderer.setWireframe(wireFrame);

	if (ImGui::Checkbox("Smooth shading (slower)", &smoothShading))
		createTerrainMesh();

	if (ImGui::DragInt("Smooth dist", &smoothDist, 0.5f))
	{
		if (smoothShading)
			createTerrainMesh();
	}

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

	
	if (ImGui::DragFloat("Mesh scale", &scale, 1.f, 1.f))
	{
		createTerrainMesh();
	}

	if (ImGui::InputInt("Sub Divs", &numSubDivs, 1, 10))
	{
		if (numSubDivs <= 0) numSubDivs = 1;
		createTerrainMesh();
	}

	ImGui::PopItemWidth();
	ImGui::End();

	if (noiser.imgui("Noise"))
		createTerrainMesh();
}

inline float lengthSqrd(const glm::vec3& a)
{
	return a.x * a.x + a.z * a.z;
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

	for (uint32_t i = 0; i < subDivs * subDivs; i++)
	{
		for (size_t v = 0; v < NUM_VERTS; v++)
		{
			glm::vec3 pos = findPos(baseVerts[v], i, subDivs) * scale;
			pos.x += scale * 0.5f;
			pos.z += scale * 0.5f;

			float noise = noiser.sample(pos.x + scroll.x, pos.z + scroll.y);
			noise = std::pow(noise, exponent);
			noise = lerpPoints.sample(noise);
			pos.y += noise;// *2.f - 1.f;
			pos.y *= amplitude;

			data.positions.emplace_back(pos);
			data.uvs.emplace_back(pos.x / scale + 0.5f, pos.z / -scale + 0.5f);
			data.indices.emplace_back((uint32_t)data.indices.size());
		}
	}

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
		

		std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
#if 0
		const float searchDistSqrd = smoothDist * ((0.5f * scale) / (float)subDivs) * smoothDist * ((0.5f * scale) / (float)subDivs);
		auto searchQuads2 = [&](int faceStartIdx, size_t curIdx, glm::vec3& sum)
		{
			if (faceStartIdx >= ((int)numPoints / 3))
				return;

			for (int j = 0; j < (int)NUM_VERTS * smoothDist * 2; j++)
			{
				int vert = faceStartIdx * 3 + j;
				if (vert < 0 || vert >= numPoints || vert == curIdx)
					continue;

				if (glm::vec3 delta = data.positions[curIdx] - data.positions[vert]; glm::dot(delta, delta) > searchDistSqrd)
					continue;

				sum += faceNormals[vert / 3];
			}
		};
		
		std::vector<size_t> points(numPoints);
		for (size_t i = 0; i < numPoints; i++)
			points[i] = i;

		const float sDist2 = (float)smoothDist * (float)smoothDist;
		data.normals.resize(numPoints);
		std::for_each(std::execution::par, points.begin(), points.end(), [&](size_t i) 
		{
			const size_t faceIdx = i / 3ull;
			glm::vec3 sum = faceNormals[faceIdx];
			const glm::vec3& iPos = data.positions[i];

#if 1
			for (size_t j = 0; j < numPoints; j++)
			{
				if (i == j || j / 3 == faceIdx)
					continue;

				const glm::vec3 delta = iPos - data.positions[j];
				const float res = lengthSqrd(delta);
				if (res > sDist2)
					continue;

				//const glm::vec3 delta = iPos - data.positions[j];
				//const float res = glm::dot(delta, delta);
				//if (res > sDist2)
				//	continue;

				sum += faceNormals[j / 3];
			}
#else
			int idx1 = (int)faceIdx - subDivs * 2 * smoothDist - (int)NUM_VERTS * smoothDist;
			searchQuads2(idx1, i, sum);

			int idx2 = (int)faceIdx - NUM_VERTS * smoothDist;
			searchQuads2(idx2, i, sum);

			int idx3 = (int)faceIdx + subDivs * 2 * smoothDist - (int)NUM_VERTS * smoothDist;
			searchQuads2(idx3, i, sum);
#endif

			data.normals[i] = glm::normalize(sum);
		});
#else
		for (size_t i = 0; i < numPoints; i++)
		{
			const size_t faceIdx = i / 3;
			glm::vec3 sum = faceNormals[faceIdx];

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
#endif
		std::chrono::duration<float> dt = std::chrono::system_clock::now() - start;
		//printf("Duration: %.7f\n", dt.count());

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
