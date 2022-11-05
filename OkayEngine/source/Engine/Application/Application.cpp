#include "Application.h"
#include "../Components/Transform.h"

Application::Application(std::wstring_view appName, uint32_t width, uint32_t height)
	:window(appName, width, height)
{
}

Application::~Application()
{
}
