#include "Editor.h"
#include "EditorHelp.h"

#include <Engine/Components/Camera.h>
#include <Engine/Components/Transform.h>
#include <Engine/Components/MeshComponent.h>


#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

Editor::Editor(std::string_view startScene)
	:Application(L"Okay"), scene(renderer), 
	gameTexture(16 * 70, 9 * 70, Okay::RenderTexture::RENDER | Okay::RenderTexture::SHADER_READ | Okay::RenderTexture::DEPTH)
	, selectionID(Okay::INVALID_UINT), selectionType(SelectionType::None), XIconID(Okay::INVALID_UINT)
{
	content.importFile("C:/Users/oliver/source/repos/Okay/OkayEditor/resources/texTest.fbx");
	content.importFile("C:/Users/oliver/source/repos/Okay/OkayEditor/resources/highPolyQuad.fbx");
	content.importFile("resources/Textures/X-icon.png");
	XIconID = (uint32_t)content.getNumTextures() - 1u; // change to getTexture() when more icons come

	Okay::Entity entity = scene.createEntity();
	entity.addComponent<Okay::MeshComponent>(0u, 0u, 0u);
	Okay::Transform& tra = entity.getComponent<Okay::Transform>();
	tra.scale *= 2.f;

	Okay::Entity camera = scene.createEntity();
	camera.addComponent<Okay::Camera>();
	camera.addComponent<Okay::PointLight>().colour = glm::vec3(1.f, 0.5f, 0.8f) * 3.f;
	scene.setMainCamera(camera);
	scene.createEntity();
	renderer.setRenderTexture(&gameTexture);

	testTex = new Okay::RenderTexture(512u, 512u, 
		Okay::RenderTexture::SHADER_WRITE | Okay::RenderTexture::SHADER_READ, Okay::RenderTexture::F_8X1);
	noiser = new Okay::PerlinNoise2D(4);
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
	gameTexture.clear(glm::vec4(0.6f, 0.2f, 0.9f, 1.f));
	Application::newFrameImGui();
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

	Application::endFrameImGui();
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
	displayStyling();

	if (Okay::Input::isKeyDown(Keys::SPACE) && Okay::Input::isKeyReleased(Keys::E))
	{
		Okay::Shader& shader = content.getShader(Okay::Entity((entt::entity)selectionID, &scene).getComponent<Okay::MeshComponent>().shaderIdx);
		//shader.pHeightMap = testTex->getSRV();
		//shader.gpuData.hasHeightMap = TRUE;
	}

	//return;
	ImGui::Begin("result", &dockSpace);

	static float tiling = 1.f;
	static float offsets[2]{};

	ImGui::DragFloat("tiling", &tiling, 0.01f, 0.00f, 100.f, "%.4f");
	ImGui::DragFloat2("offset", offsets, 0.01f, 0.00f, 100.f, "%.4f");
		
	ImGui::Image(testTex->getSRV(), ImVec2(512.f, 512.f), ImVec2(offsets[0], offsets[1]), ImVec2(offsets[0] + tiling, offsets[1] + tiling));
	ImGui::End();

	if (noiser->imgui("Perlin"))
		noiser->generateTexture(testTex->getBuffer());
}

void Editor::displayEntities()
{
	static bool open = true;
	ImGui::Begin("Entities", &open);

	entt::registry& reg = scene.getRegistry();

	if (ImGui::Button("Create"))
	{
		selectionID = (uint32_t)scene.createEntity().getID();
		selectionType = SelectionType::Entity;
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

	auto entities = reg.view<Okay::Transform>();

	for (auto entity : entities)
	{
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


	ImGui::Begin("Other", &open);
	ImGui::PushItemWidth(-15.f);

	static float timer = 0.f;
	static float dtSum = 0.f;
	static int frameCount = 0;
	static float dispDt = Okay::Time::getApplicationDT();

	dtSum += Okay::Time::getApplicationDT();
	frameCount++;
	if ((timer += Okay::Time::getApplicationDT()) > 0.5f)
	{
		dispDt = dtSum / frameCount;
		timer = 0.f;
		dtSum = 0.f;
		frameCount = 0;
	}

	ImGui::Text("FPS: %.6f", 1.f / dispDt);
	ImGui::Text("MS:  %.6f", dispDt);

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
		{
			char output[Window::MAX_FILENAME_LENGTH]{};
			if (window.openFileExplorer(output, Window::MAX_FILENAME_LENGTH))
				content.importFile(output);
		}
	
		if (ImGui::MenuItem("weow"))
			printf("weow");
	
		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();

	displayAssetList();

	ImGui::End();
}

void Editor::displayStyling()
{
	if (!ImGui::Begin("Styling"))
	{
		ImGui::End();
		return;
	}
	static float colEditWidth = -325.f;
	static float buttonWidth = 175.f;
	static float buttonOffset = 20.f;
	ImGui::PushItemWidth(colEditWidth);

	/*ImGui::DragFloat("ColorEdit width", &colEditWidth);
	ImGui::DragFloat("Button Width", &buttonWidth);
	ImGui::DragFloat("Button Offset", &buttonOffset);
	ImGui::Separator();*/

	ImGuiStyle& style = ImGui::GetStyle();
	IMGUI_DISPLAY_STYLE_COLOUR(Text);
	IMGUI_DISPLAY_STYLE_COLOUR(TextDisabled);
	IMGUI_DISPLAY_STYLE_COLOUR(WindowBg);
	IMGUI_DISPLAY_STYLE_COLOUR(ChildBg);
	IMGUI_DISPLAY_STYLE_COLOUR(PopupBg);
	IMGUI_DISPLAY_STYLE_COLOUR(Border);
	IMGUI_DISPLAY_STYLE_COLOUR(BorderShadow);
	IMGUI_DISPLAY_STYLE_COLOUR(FrameBg);
	IMGUI_DISPLAY_STYLE_COLOUR(FrameBgHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(FrameBgActive);
	IMGUI_DISPLAY_STYLE_COLOUR(TitleBg);
	IMGUI_DISPLAY_STYLE_COLOUR(TitleBgActive);
	IMGUI_DISPLAY_STYLE_COLOUR(TitleBgCollapsed);
	IMGUI_DISPLAY_STYLE_COLOUR(MenuBarBg);
	IMGUI_DISPLAY_STYLE_COLOUR(ScrollbarBg);
	IMGUI_DISPLAY_STYLE_COLOUR(ScrollbarGrab);
	IMGUI_DISPLAY_STYLE_COLOUR(ScrollbarGrabHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(ScrollbarGrabActive);
	IMGUI_DISPLAY_STYLE_COLOUR(CheckMark);
	IMGUI_DISPLAY_STYLE_COLOUR(SliderGrab);
	IMGUI_DISPLAY_STYLE_COLOUR(SliderGrabActive);
	IMGUI_DISPLAY_STYLE_COLOUR(Button);
	IMGUI_DISPLAY_STYLE_COLOUR(ButtonHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(ButtonActive);
	IMGUI_DISPLAY_STYLE_COLOUR(Header);
	IMGUI_DISPLAY_STYLE_COLOUR(HeaderHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(HeaderActive);
	IMGUI_DISPLAY_STYLE_COLOUR(SeparatorHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(SeparatorActive);
	IMGUI_DISPLAY_STYLE_COLOUR(ResizeGrip);
	IMGUI_DISPLAY_STYLE_COLOUR(ResizeGripHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(ResizeGripActive);
	IMGUI_DISPLAY_STYLE_COLOUR(Tab);
	IMGUI_DISPLAY_STYLE_COLOUR(TabHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(TabActive);
	IMGUI_DISPLAY_STYLE_COLOUR(TabUnfocused);
	IMGUI_DISPLAY_STYLE_COLOUR(TabUnfocusedActive);
	IMGUI_DISPLAY_STYLE_COLOUR(DockingPreview);
	IMGUI_DISPLAY_STYLE_COLOUR(DockingEmptyBg);
	IMGUI_DISPLAY_STYLE_COLOUR(PlotLines);
	IMGUI_DISPLAY_STYLE_COLOUR(PlotLinesHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(PlotHistogram);
	IMGUI_DISPLAY_STYLE_COLOUR(PlotHistogramHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(TextSelectedBg);
	IMGUI_DISPLAY_STYLE_COLOUR(DragDropTarget);
	IMGUI_DISPLAY_STYLE_COLOUR(NavHighlight);
	IMGUI_DISPLAY_STYLE_COLOUR(NavWindowingHighlight);
	IMGUI_DISPLAY_STYLE_COLOUR(NavWindowingDimBg);
	IMGUI_DISPLAY_STYLE_COLOUR(ModalWindowDimBg);

	ImGui::PopItemWidth();
	ImGui::End();
}