#pragma once
#include "ResourceManager.h"
#include "Renderer.h"

/*

	Go down from engine and see what needs what
	Remove unnecessary includes
	Make Renderer, Assets & DX11 not single-tons
	Limit access as much as possible (Like camera doesn't need DX11, only Defins.h)
	
	Go down from engine and see what needs what
	Remove unnecessary includes
	Make Renderer, Assets & DX11 not single-tons
	Limit access as much as possible (Like camera doesn't need DX11, only Defins.h)
	
	Go down from engine and see what needs what
	Remove unnecessary includes
	Make Renderer, Assets & DX11 not single-tons
	Limit access as much as possible (Like camera doesn't need DX11, only Defins.h)
	
	Go down from engine and see what needs what
	Remove unnecessary includes
	Make Renderer, Assets & DX11 not single-tons
	Limit access as much as possible (Like camera doesn't need DX11, only Defins.h)
	
	Go down from engine and see what needs what
	Remove unnecessary includes
	Make Renderer, Assets & DX11 not single-tons
	Limit access as much as possible (Like camera doesn't need DX11, only Defins.h)
	
	Go down from engine and see what needs what
	Remove unnecessary includes
	Make Renderer, Assets & DX11 not single-tons
	Limit access as much as possible (Like camera doesn't need DX11, only Defins.h)
	


*/

namespace Okay
{
	class Engine
	{
	private:
		Engine();
		static Engine& Get()
		{
			static Engine engine;
			return engine;
		}
	public:
		~Engine();
		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(const Engine&) = delete;

		static Renderer& GetRenderer() { return Get().renderer; }
		static Assets& GetAssets() { return Get().assets; }
		static DX11& GetDX11() { return Get().dx11; }

		static void NewFrame();
		static void EndFrame();

		static bool LoadScene(const Okay::String& sceneName);
		static bool LoadScene(UINT sceneIndex);

		static Scene* GetActiveScene() { return Get().activeScene.get(); }

	private:
		Renderer& renderer;
		Assets& assets;
		DX11& dx11;

	private: // Scenes
		static const Okay::String SceneDecleration;
		std::unique_ptr<Scene> activeScene;

		static void ReadEntity(Entity& entity, std::ifstream& reader);
		static void ReadComponentData(Entity& entity, Components type, std::ifstream& reader);
	};

}