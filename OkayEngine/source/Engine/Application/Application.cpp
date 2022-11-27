#include "Application.h"
#include "Engine/DirectX/DX11.h"
#include "Engine/Components/Transform.h"

Application::Application(std::wstring_view appName, uint32_t width, uint32_t height)
	:window(width, height)
{
	// Make sure DX11 is set up and set window name after DX11 has found it
	DX11::getInstance();
	window.setName(appName);
}

Application::~Application()
{
}
