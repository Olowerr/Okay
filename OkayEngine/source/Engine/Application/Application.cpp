#include "Application.h"
#include "Engine/DirectX/DX11.h"

namespace Okay
{
	Application::Application(const wchar_t* appName, uint32_t width, uint32_t height)
		:window(width, height, appName, Okay::RenderTexture::RENDER | Okay::RenderTexture::DEPTH | Okay::RenderTexture::SHADER_READ | Okay::RenderTexture::SHADER_WRITE),
		renderer(&window.getRenderTexture(), &scene)
	{
		Renderer::init();
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

			renderer.render(); 
			renderer.realRender();

			postRender();

			window.present();
		}

		end();
	}	
}

