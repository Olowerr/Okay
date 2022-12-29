#include "Editor.h"

#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/MeshComponent.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

Editor::Editor(std::string_view startScene)
	:Application(L"Okay"), scene(renderer)
{
	content.importFile("C:/Users/olive/source/repos/Okay/OkayEditor/resources/texTest.fbx");
	content.importFile("C:/Users/olive/source/repos/Okay/OkayEditor/resources/axis.fbx");

	Okay::Entity entity = scene.createEntity();
	entity.addComponent<Okay::MeshComponent>(0u, 0u);
	Okay::Transform& tra = entity.getComponent<Okay::Transform>();
	tra.scale *= 2.f;

	Okay::Entity entity2 = scene.createEntity();
	entity2.addComponent<Okay::MeshComponent>(1u, 1u);
	Okay::Transform& tra2 = entity2.getComponent<Okay::Transform>();
	tra2.position.x = 5.f;

	Okay::Entity camera = scene.createEntity();
	camera.addComponent<Okay::Camera>();
	camera.addComponent<Okay::PointLight>().colour = glm::vec3(1.f, 0.5f, 0.8f) * 3.f;
	scene.setMainCamera(camera);

}

Editor::~Editor()
{
	
}
   
void Editor::run()
{
	Application::initImgui();

	using namespace Okay;
	DX11& dx11 = DX11::getInstance();
	Transform& tra = scene.getMainCamera().getComponent<Transform>();
	tra.rotation.x = glm::pi<float>() * 0.25f;
	
	scene.start();
	Time::start();

	RenderTexture rt;
	while (window.isOpen())
	{
		// New frame
		newFrame();

		update();
		// Update
		scene.update();

		tra.rotation.y += Time::getDT();
		tra.calculateMatrix();
		tra.position = tra.forward() * -5.f;

		static bool open = true;
		ImGui::Begin("Hello", &open);
		ImGui::Text("Tax");
		ImGui::End();

		if (Input::isKeyDown(Keys::A))
			printf("A DOWN\n");
		if (Input::isKeyReleased(Keys::N))
			printf("N RELEASED\n");
		if (Input::isKeyPressed(Keys::Z))
			printf("Z PRESSED\n");

		static Window* window = nullptr;
		if (Input::isKeyPressed(Keys::ONE))
		{
			window = new Window(300, 300, L"New window", RenderTexture::RENDER, true);
			window->clear(glm::vec4(1.f, 0.f, 0.f, 0.f));
			window->present();
			printf("One\n");
		}
		if (Input::isKeyPressed(Keys::TWO))
		{
			delete window;
			printf("Two\n");
		}


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
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Editor::endFrame()
{
	//DX11::getInstance().getDeviceContext()->OMSetRenderTargets(1, window.getRenderTexture().getRTV(), nullptr);

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

}
