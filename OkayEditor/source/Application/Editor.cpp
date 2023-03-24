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
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.Fonts->AddFontFromFileTTF("C://Windows/Fonts/Arial.ttf", 14.f);

	setStyle();

	ImGui_ImplWin32_Init(window.getHWnd());
	ImGui_ImplDX11_Init(DX11::get().getDevice(), DX11::get().getDeviceContext());


	using namespace Okay;

	renderer.setRenderTexture(&gameTexture);
	gameTexture.addOnResizeCallback(&Scene::updateCamerasAspectRatio, &scene);

	editorCamera = scene.createEntity();
	editorCamera.addComponent<EditorEntity>();
	editorCamera.addScript<EditorCamera>();

	renderer.setCustomCamera(editorCamera);

	content.importFile("resources/highPolyQuad.fbx");

	Entity entity = scene.createEntity();
	entity.addComponent<MeshComponent>(0u, 0u, 0u);
	entity.getComponent<Transform>().scale *= 2.f;


	Entity floor = scene.createEntity();
	floor.addComponent<MeshComponent>(1u);
	floor.getComponent<Transform>().scale *= 10.f;
	floor.getComponent<Transform>().position.y = -5.f;


	Entity light = scene.createEntity();
	light.addComponent<DirectionalLight>();
	Transform& lightTra = light.getComponent<Transform>();
	lightTra.position = glm::vec3(2.f);
	lightTra.rotation = glm::radians(glm::vec3(50.f, -60.f, 0.f));

	SkyLight& skyComp = light.addComponent<SkyLight>(); 
	skyComp.skyBox->create("resources/Textures/SkyBox1.png");
	skyComp.sunColour = glm::vec3(0.86f, 0.55f, 0.47f);
	skyComp.sunSize = 99.9f;
	skyComp.ambientTint = glm::vec3(0.7f, 0.8f, 0.9);
	skyComp.ambientIntensity = 0.8f;

	scene.setSkyLight(light);
}

Editor::~Editor()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Editor::update()
{
	newImGuiFrame();
	scene.update();
	displaySceneSettings();
	displayEntities();
	displayInspector();
	displayContent();
	displayStyling();

	endImGuiFrame();
}

void Editor::postRender()
{
	DX11::get().getDeviceContext()->OMSetRenderTargets(1u, window.getRenderTexture().getRTV(), nullptr);

	ImGui::PopFont();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

void Editor::newImGuiFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void Editor::endImGuiFrame()
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

void Editor::setStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.07f, 0.53f, 0.62f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.03f, 0.32f, 0.45f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.03f, 0.24f, 0.34f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.89f, 0.31f, 0.06f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.39f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.60f, 0.26f, 0.04f, 0.39f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
	style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	style.GrabRounding = style.FrameRounding = 2.3f;
}