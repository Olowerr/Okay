#pragma once
#include "Window.h"

class Application
{
public:
	Application(std::wstring_view appName, uint32_t width = 1600u, uint32_t height = 900u);
	virtual ~Application();

	virtual void run() = 0;

protected:
	Window window;
};