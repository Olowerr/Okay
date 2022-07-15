#pragma once
#include "ResourceManager.h"
#include "Renderer.h"

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

		static void NewFrame();
		static void EndFrame();

		static bool LoadScene(const Okay::String& sceneName);
		static bool LoadScene(UINT sceneIndex);

	private:
		Renderer& renderer;
		Assets& assets;
		DX11& dx11;

	private: // Scenes
		const Okay::String SceneDecleration = "../Content/Scenes/SceneDecleration.okayDec";
		std::unique_ptr<Scene> activeScene;
	};
}