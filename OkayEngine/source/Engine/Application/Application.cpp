#include "Application.h"
#include "Engine/DirectX/DX11.h"

#include "Engine/Components/MeshComponent.h"

#include <execution>

namespace Okay
{
	Application::Application(const wchar_t* appName, uint32_t width, uint32_t height)
		:window(width, height, appName, Okay::RenderTexture::RENDER | Okay::RenderTexture::DEPTH | Okay::RenderTexture::SHADER_READ | Okay::RenderTexture::SHADER_WRITE),
		renderer(&window.getRenderTexture(), &scene)
	{
		Renderer::init();
		registerRenderer(&renderer);
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

#ifdef MULTI_THREADED
			// EnTT is not set up for multithreading, using views in parallel generates crashes. 
			// Meaning submitting entities for rendering has to be done for one renderer at a time.
			for (size_t i = 0; i < registeredRenderers.size(); i++)
				registeredRenderers[i]->prepareForRecording();
				
			// Each renderer can record their commands simultaneously on their own context
			std::for_each(std::execution::par, registeredRenderers.begin(), registeredRenderers.end(), [&](Renderer* pRenderer)
			{
				pRenderer->recordCommands();
			});

			// The actual execution is done on the immediate context
			for (size_t i = 0; i < registeredRenderers.size(); i++)
				registeredRenderers[i]->executeCommands();
#else		
			for (size_t i = 0; i < registeredRenderers.size(); i++)
			{
				registeredRenderers[i]->prepareForRecording();
				registeredRenderers[i]->recordCommands();
				registeredRenderers[i]->executeCommands();
			}
#endif
			postRender();

			window.present();
		}

		end();
	}

	void Application::registerRenderer(Renderer* pRenderer)
	{
		registeredRenderers.emplace_back(pRenderer);
	}

	void Application::deRegisterRenderer(Renderer* pRenderer)
	{
		for (size_t i = 0; i < registeredRenderers.size(); i++)
		{
			if (registeredRenderers[i] == pRenderer)
				registeredRenderers.erase(registeredRenderers.begin() + i);
		}
	}

}

