#include "Application.h"
#include "Engine/DirectX/DX11.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

Application::Application(const wchar_t* appName, uint32_t width, uint32_t height)
	:window(width, height, L"Okay Engine", Okay::RenderTexture::RENDER | Okay::RenderTexture::DEPTH),
	renderer(&window.getRenderTexture(), content)
{
	// Make sure DX11 is set up, then set window name after DX11 has found it
	DX11::getInstance();
	window.setName(appName);
}

Application::~Application()
{
}

void Application::initImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(window.getHWnd());
	ImGui_ImplDX11_Init(DX11::getInstance().getDevice(), DX11::getInstance().getDeviceContext());
}

void Application::destroyImgui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Application::newFrame()
{
	window.update();
	window.clear();
	renderer.newFrame();

	Okay::Time::measure();
}

void Application::endFrame()
{
	window.present();
}
