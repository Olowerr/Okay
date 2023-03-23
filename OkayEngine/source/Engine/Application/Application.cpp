#include "Application.h"
#include "Engine/DirectX/DX11.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

namespace Okay
{
	Application::Application(const wchar_t* appName, uint32_t width, uint32_t height)
		:window(width, height, appName, Okay::RenderTexture::RENDER | Okay::RenderTexture::DEPTH | Okay::RenderTexture::SHADER_READ | Okay::RenderTexture::SHADER_WRITE),
		renderer(&window.getRenderTexture())
	{
		Renderer::init();
		renderer.setScene(&scene);
	}

	Application::~Application()
	{
	}

	void Application::start()
	{
		scene.start();
	}

	void Application::update()
	{
		scene.update();
	}

	void Application::end()
	{
		scene.end();
	}

	void Application::postRender()
	{
	}

	void Application::run()
	{
		start();
		Time::start();

		while (window.isOpen())
		{
			Time::measure();
			window.update();

			update();

			renderer.newFrame();
			renderer.render(); 

			postRender();

			window.present();
		}

		end();
	}	
}

