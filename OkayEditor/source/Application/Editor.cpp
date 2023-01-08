#include "Editor.h"

#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/MeshComponent.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

Editor::Editor(std::string_view startScene)
	:Application(L"Okay"), scene(renderer), 
	gameTexture(160 * 7, 90 * 7, Okay::RenderTexture::RENDER | Okay::RenderTexture::SHADER_READ | Okay::RenderTexture::DEPTH)
	, selectionID(Okay::INVALID_UINT), selectionType(SelectionType::None), XIconID(Okay::INVALID_UINT)
{
	content.importFile("C:/Users/oliver/source/repos/Okay/OkayEditor/resources/texTest.fbx");
	content.importFile("resources/Textures/X-icon.png");
	XIconID = (uint32_t)content.getNumTextures() - 1u; // change to getTexture() when more icons come

	Okay::Entity entity = scene.createEntity();
	entity.addComponent<Okay::MeshComponent>(0u, 0u);
	Okay::Transform& tra = entity.getComponent<Okay::Transform>();
	tra.scale *= 2.f;

	Okay::Entity camera = scene.createEntity();
	camera.addComponent<Okay::Camera>();
	camera.addComponent<Okay::PointLight>().colour = glm::vec3(1.f, 0.5f, 0.8f) * 3.f;
	scene.setMainCamera(camera);
	scene.createEntity();
	renderer.setRenderTexture(&gameTexture);
}

Editor::~Editor()
{
	
}
   
void Editor::run()
{
	using namespace Okay;

	Application::initImgui();

	DX11& dx11 = DX11::getInstance();
	Transform& tra = scene.getMainCamera().getComponent<Transform>();
	tra.rotation.x = glm::pi<float>() * 0.25f;
	
	scene.start();
	Time::start();

	while (window.isOpen())
	{
		// New frame
		newFrame();

		// Update
		update();
		scene.update();

		//tra.rotation.y += Time::getDT();
		tra.calculateMatrix();
		tra.position = tra.forward() * -5.f;

		if (Input::isKeyDown(Keys::A))
			printf("A DOWN\n");
		if (Input::isKeyReleased(Keys::N))
			printf("N RELEASED\n");
		if (Input::isKeyPressed(Keys::Z))
			printf("Z PRESSED\n");

		// Submit & render
		scene.submit();
		renderer.render(scene.getMainCamera());

		// End frame
		endFrame();
	} 
	scene.end();

	Application::destroyImgui();
}

void Editor::newFrame()
{
	Application::newFrame();
	gameTexture.clear(glm::vec4(0.6f, 0.2f, 0.9f, 1.f));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Editor::endFrame()
{
	static bool open = true;
	static ImVec2 viewp = VEC2_GLM_TO_IMGUI(gameTexture.getDimensions());
	ImGui::Begin("Viewport", &open);
	ImGui::Image(gameTexture.getSRV(), viewp);
	ImGui::End();

	// ((const Window&)window) bruh
	DX11::getInstance().getDeviceContext()->OMSetRenderTargets(1, ((const Window&)window).getRenderTexture().getRTV(), nullptr);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	Application::endFrame();
}

void Editor::update()
{
	static bool dockSpace = true;

	//ImGuiWindowFlags_NoMove
	//ImGuiWindowFlags_NoBackground
	//ImGuiWindowFlags_NoTitleBar

	if (dockSpace)
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());


	displayEntities();
	displayInspector();
	displayContent();
}

void Editor::displayEntities()
{
	static bool open = true;
	ImGui::Begin("Entities", &open);

	entt::registry& reg = scene.getRegistry();

	if (!ImGui::BeginListBox("##EntNoLabel", { ImGui::GetWindowSize().x, -1.f }))
	{
		ImGui::EndListBox();
		ImGui::End();
		return;
	}

	auto entities = reg.view<Okay::Transform>();

	for (auto entity : entities)
	{
		//auto [transform] = entities.get(entity);

		if (ImGui::Selectable(std::to_string((uint32_t)entity).c_str(), entity == (entt::entity)selectionID && selectionType == SelectionType::Entity))
		{
			selectionID = (uint32_t)entity;
			selectionType = SelectionType::Entity;
		}

		//if (ImGui::Selectable(entities.get<CompTag>(entity).tag, entity == currentEntity))
			//{
			//	currentEntity = Entity(entity, Engine::GetActiveScene());
			//	UpdateSelection(AssetType::ENTITY);
			//}
			//
			//if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			//{
			//	currentEntity = Entity(entity, Engine::GetActiveScene());
			//	UpdateSelection(AssetType::ENTITY);
			//
			//	entityMenu = true;
			//	listMenu = false;
			//	menuPos = ImGui::GetMousePos();
			//}
	}

	//if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !entityMenu && ImGui::IsWindowHovered())
	//{
	//	listMenu = true;
	//	menuPos = ImGui::GetMousePos();
	//}

	ImGui::EndListBox();
	ImGui::End();
}

void Editor::displayInspector()
{
	static bool open = true;
	ImGui::Begin("Inspector", &open);
	ImGui::PushItemWidth(-15.f);

	switch (selectionType)
	{
	case Editor::SelectionType::None:
		break;
	case Editor::SelectionType::Entity:
	{
		Okay::Entity entity((entt::entity)selectionID, &scene);
		displayComponents(entity);
		addComponents(entity);
	}
		break;
	case Editor::SelectionType::Mesh:
		displayMesh(selectionID);
		break;
	case Editor::SelectionType::Material:
		displayMaterial(selectionID);
		break;
	case Editor::SelectionType::Texture:
		displayTexture(selectionID);
		break;
	default:
		break;
	}

	ImGui::PopItemWidth();
	ImGui::End();
}

void Editor::displayContent()
{
	static bool open = true;
	ImGui::Begin("Content Browser", &open, ImGuiWindowFlags_MenuBar);

	ImGui::BeginMenuBar();
	if (ImGui::BeginMenu("Options"))
	{
		if (ImGui::MenuItem("Import"))
			openFileExplorer();
	
		if (ImGui::MenuItem("weow"))
			printf("weow");
	
		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();

	displayAssetList();

	ImGui::End();
}

void Editor::openFileExplorer()
{
	const size_t MaxFileLength = 512;
	OPENFILENAME ofn{};

	wchar_t fileName[MaxFileLength]{};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = window.getHWnd();
	ofn.lpstrFile = fileName;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MaxFileLength;
	ofn.lpstrFilter = L"All Files\0*.*";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_NOCHANGEDIR;

	if (!GetOpenFileName(&ofn))
		return;

	char text[MaxFileLength]{};
	wcstombs_s(nullptr, text, MaxFileLength, ofn.lpstrFile, MaxFileLength);
	content.importFile(text);
}
