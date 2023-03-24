#pragma once
#include "Window.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/ContentBrowser.h"

#include "Scene.h"

#include "Time.h"
#include "Input/Input.h"

namespace Okay
{
	class Application
	{
	public:
		Application(const wchar_t* appName, uint32_t width = 1600u, uint32_t height = 900u);
		virtual ~Application();

		virtual void start();
		virtual void update();
		virtual void end();

		virtual void postRender();
		virtual void run();

		void registerRenderer(Ref<Renderer> pRenderer);
		void deregisterRenderer(Ref<Renderer> pRenderer);

	protected:
		Window window;
		Ref<Renderer> renderer;
		Ref<Scene> scene;

		std::vector<Ref<Renderer>> registeredRenderers;
	};
}
