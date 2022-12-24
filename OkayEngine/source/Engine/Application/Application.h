#pragma once
#include "Window.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/ContentBrowser.h"
#include "Engine/Components/Camera.h"
#include "Time.h"
#include "Input.h"

class Application
{
public:
	Application(std::wstring_view appName, uint32_t width = 1600u, uint32_t height = 900u);
	virtual ~Application();

	virtual void run() = 0;
	
	void newFrame();
	void endFrame();

protected:
	Window window;
	Okay::Renderer renderer;
	Okay::ContentBrowser content;
};