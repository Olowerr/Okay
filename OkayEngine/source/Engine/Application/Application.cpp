#include "Application.h"
#include "Engine/DirectX/DX11.h"

Application::Application(std::wstring_view appName, uint32_t width, uint32_t height)
	:window(width, height), renderer(content)
{
	// Make sure DX11 is set up, then set window name after DX11 has found it
	DX11::getInstance();
	window.setName(appName);
	Okay::Input::setTargetWindow(window.getHWnd());
}

Application::~Application()
{
}

void Application::newFrame()
{
	window.update();
	DX11::getInstance().clear();
	renderer.newFrame();

	Okay::Time::measure();
}

void Application::endFrame()
{
	DX11::getInstance().present();
}
