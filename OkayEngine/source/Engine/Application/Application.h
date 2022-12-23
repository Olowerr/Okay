#pragma once
#include "Window.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/ContentBrowser.h"
#include "Engine/Components/Camera.h"

#include <chrono>

class Application
{
public:
	Application(std::wstring_view appName, uint32_t width = 1600u, uint32_t height = 900u);
	virtual ~Application();

	virtual void run() = 0;
	

protected:
	Window window;
	Okay::Renderer renderer;
	Okay::ContentBrowser content;
	
	std::chrono::time_point<std::chrono::system_clock> frameStart;
	std::chrono::duration<float> deltaTime;
};