#include "Application.h"
#include "Engine/DirectX/DX11.h"
#include "Engine/Components/Transform.h"

Application::Application(std::wstring_view appName, uint32_t width, uint32_t height)
	:window(appName, width, height)
{
	DX11::initialize(&window);
}

Application::~Application()
{
}
