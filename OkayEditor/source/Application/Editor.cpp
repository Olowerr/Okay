#include "Editor.h"
#include "EditorHelp.h"
#include "../Scripts/EditorCameraMovement.h"

#include <Engine/Components/Camera.h>
#include <Engine/Components/Transform.h>
#include <Engine/Components/MeshComponent.h>
#include <Engine/Components/SkyLight.h>

#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

Editor::Editor(std::string_view startScene)
	:Application(L"Okay"), content(Okay::ContentBrowser::get())
	, gameTexture(16 * 70, 9 * 70, Okay::RenderTexture::RENDER | Okay::RenderTexture::SHADER_READ | Okay::RenderTexture::DEPTH)
	, selectionID(Okay::INVALID_UINT), selectionType(SelectionType::None)
{
	using namespace Okay;

	renderer.setRenderTexture(&gameTexture);
	gameTexture.addOnResizeCallback(&Scene::updateCamerasAspectRatio, &scene);

	editorCamera = scene.createEntity();
	editorCamera.addComponent<EditorEntity>();
	editorCamera.addScript<EditorCamera>();

	content.importFile("resources/highPolyQuad.fbx");

	Entity entity = scene.createEntity();
	entity.addComponent<MeshComponent>(0u, 0u, 0u);
	entity.getComponent<Transform>().scale *= 2.f;

	Entity floor = scene.createEntity();
	floor.addComponent<MeshComponent>(1u);
	floor.getComponent<Transform>().scale *= 10.f;
	floor.getComponent<Transform>().position.y = -5.f;

	Entity light = scene.createEntity();
	light.getComponent<Transform>().scale *= 0.5f;
	light.getComponent<Transform>().position = glm::vec3(2.f);
	light.addComponent<PointLight>().intensity = 2.f;

	testTex = new RenderTexture(512u, 512u, RenderTexture::SHADER_WRITE | RenderTexture::SHADER_READ, RenderTexture::F_8X1);
	noiser = new PerlinNoise2D(4);
	noiser->generateTexture(testTex->getBuffer());
}

Editor::~Editor()
{
	delete testTex;
	delete noiser;
}
   
void Editor::run()
{
	using namespace Okay;

	Application::initImgui();

	scene.start();
	Time::start();

	renderer.setScene(&scene);
	while (window.isOpen())
	{
		newFrame();

		update();
		scene.update();

		renderer.imGui();
		renderer.render(editorCamera);

		endFrame();
	} 
	scene.end();

	Application::destroyImgui();
}

void Editor::newFrame()
{
	gameTexture.clear(glm::vec4(0.6f, 0.2f, 0.9f, 1.f));
	Application::newFrameImGui();
}

void Editor::endFrame()
{
	static ImVec2 texSize = VEC2_GLM_TO_IMGUI(gameTexture.getDimensions());
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
	ImGui::Begin("Viewport", nullptr);

	const ImVec2 winSize = ImGui::GetContentRegionMax();
	if (winSize.x != texSize.x || winSize.y != texSize.y)
	{
		texSize = winSize;
		gameTexture.resize((uint32_t)winSize.x, (uint32_t)winSize.y);
	}

	ImGui::Image(*gameTexture.getSRV(), texSize);

	ImGui::End();
	ImGui::PopStyleVar();

	DX11::get().getDeviceContext()->OMSetRenderTargets(1, window.getRenderTexture().getRTV(), nullptr);
	Application::endFrameImGui();
}

void Editor::update()
{
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	displaySceneSettings();
	displayEntities();
	displayInspector();
	displayContent();
	displayStyling();
	
	
	return;

	// Perlin noise testing
	ImGui::Begin("result", nullptr);

	static float tiling = 1.f;
	static float offsets[2]{};

	ImGui::DragFloat("tiling", &tiling, 0.01f, 0.00f, 100.f, "%.4f");
	ImGui::DragFloat2("offset", offsets, 0.01f, 0.00f, 100.f, "%.4f");
		
	ImGui::Image(*testTex->getSRV(), ImVec2(512.f, 512.f), ImVec2(offsets[0], offsets[1]), ImVec2(offsets[0] + tiling, offsets[1] + tiling));
	ImGui::End();

	if (noiser->imgui("Perlin"))
		noiser->generateTexture(testTex->getBuffer());
}

void Editor::displayEntities()
{
	ImGui::Begin("Entities", nullptr);

	if (ImGui::Button("Create"))
	{
		selectionID = (uint32_t)scene.createEntity().getID();
		selectionType = SelectionType::Entity;
		editorCamera.getScript<EditorCamera>().setSelectedEntity(getEntity(selectionID));
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		if (selectionType == SelectionType::Entity)
		{
			scene.destroyEntity(entt::entity(selectionID));
			selectionType = SelectionType::None;
			selectionID = Okay::INVALID_UINT;
		}
	}

	if (!ImGui::BeginListBox("##EntNoLabel", { ImGui::GetWindowSize().x, -1.f }))
	{
		ImGui::EndListBox();
		ImGui::End();
		return;
	}

	auto entities = scene.getRegistry().view<Okay::Transform>(entt::exclude<EditorEntity>);

	for (auto entity : entities)
	{
		if (ImGui::Selectable(std::to_string((uint32_t)entity).c_str(), entity == (entt::entity)selectionID && selectionType == SelectionType::Entity))
		{
			selectionID = (uint32_t)entity;
			selectionType = SelectionType::Entity;
			editorCamera.getScript<EditorCamera>().setSelectedEntity(getEntity(selectionID));
		}
	}

	ImGui::EndListBox();
	ImGui::End();
}

void Editor::displayInspector()
{
	ImGui::Begin("Inspector", nullptr);
	ImGui::PushItemWidth(-15.f);

	switch (selectionType)
	{
	case Editor::SelectionType::None:
		break;
	case Editor::SelectionType::Entity:
	{
		Okay::Entity entity = getEntity(selectionID);
		displayComponents(entity);
		addComponents(entity);
	}
		break;
	case SelectionType::Mesh:
		displayMesh(selectionID);
		break;
	case SelectionType::Material:
		displayMaterial(selectionID);
		break;
	case SelectionType::Texture:
		displayTexture(selectionID);
		break;
	case SelectionType::Shader:
		displayShader(selectionID);
		break;
	default:
		break;
	}

	ImGui::PopItemWidth();
	ImGui::End();


	ImGui::Begin("Other", nullptr);
	ImGui::PushItemWidth(-15.f);

	static float dtSum = 0.f;
	static int frameCount = 0;
	static float avgDt = 1.f;

	frameCount++;
	if ((dtSum += Okay::Time::getApplicationDT()) > 0.5f)
	{
		avgDt = dtSum / frameCount;
		dtSum -= 0.5f;
		frameCount = 0;
	}

	ImGui::Text("FPS: %.6f", 1.f / avgDt);
	ImGui::Text("MS:  %.6f", avgDt * 1000.f);

	ImGui::PopItemWidth();
	ImGui::End();
}

void Editor::displayContent()
{
	ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_MenuBar);

	ImGui::BeginMenuBar();
	if (ImGui::BeginMenu("Options"))
	{
		if (ImGui::MenuItem("Import"))
		{
			std::string file;
			if (window.fileExplorerSelectFile(file))
				content.importFile(file);
		}
	
		if (ImGui::MenuItem("weow"))
			printf("weow");
	
		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();

	displayAssetList();

	ImGui::End();
}

void Editor::displaySceneSettings()
{
	if (!ImGui::Begin("Scene Settings"))
	{
		ImGui::End();
		return;
	}

	if (Okay::Entity entity = selectEntity<Okay::Camera>("Main Camera", scene.getMainCamera().getID(), [&]() {scene.setMainCamera(Okay::Entity()); }))
		scene.setMainCamera(entity);

	if (Okay::Entity entity = selectEntity<Okay::SkyLight>("Sky Light", scene.getSkyLight().getID(), [&]() { scene.setSkyLight(Okay::Entity()); }))
		scene.setSkyLight(entity);
	

	ImGui::End();
}
