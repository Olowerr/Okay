#pragma once
#include "ResourceManager.h"
#include "Renderer.h"
#include "Entity.h"
#include <iostream>

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

		static void Initialize();

		static Renderer& GetRenderer()	{ return Get().renderer; }
		static Assets& GetAssets()		{ return Get().assets; }

		static void NewFrame();
		static void EndFrame();
		static void Update();

		static bool LoadScene(const Okay::String& sceneName);
		static bool LoadScene(UINT sceneIndex);

		static bool SaveCurrentScene(); 

		static Scene* GetActiveScene() { return Get().activeScene.get(); }

	private:
		Renderer renderer;
		Assets assets;

	private: // Scenes
		static const Okay::String SceneDecleration;
		std::unique_ptr<Scene> activeScene;

		static void ReadEntity(Entity& entity, std::ifstream& reader);
		static void ReadComponentData(Entity& entity, Components type, std::ifstream& reader);
	};

}