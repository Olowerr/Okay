#pragma once
#include "Window.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/ContentBrowser.h"

#include "Time.h"
#include "Input/Input.h"

class Application
{
public:
	Application(const wchar_t* appName, uint32_t width = 1600u, uint32_t height = 900u);
	virtual ~Application();

	virtual void run() = 0;
	
	void newFrame();
	void endFrame();

protected:
	Window window;
	Okay::Renderer renderer;
	Okay::ContentBrowser content;
};