#pragma once
#include <iostream>
#include <chrono>

#include "ResourceManager.h"
#include "Renderer.h"
#include "Entity.h"
#include "Keys.h"


#define COUNT_COMP(T) numComp += (bool)registry.try_get<Okay::T>(entity);

#define WRITE_DATA(T)												\
if (auto* tPtr = registry.try_get<Okay::T>(entity))					\
{																	\
	Okay::Components type = Okay::T::ID;							\
	writer.write((const char*)&type, sizeof(Okay::Components));		\
	tPtr->WritePrivateData(writer);									\
}

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
		static void ResizeScreen();

		static Renderer& GetRenderer()	{ return Get().renderer; }
		static Assets& GetAssets()		{ return Get().assets; }

		static void NewFrame();
		static void EndFrame();
		
		static void StartScene();
		static void Update();
		static void EndScene();

		static void Render();

		static bool LoadScene(const Okay::String& sceneName);
		static bool LoadScene(UINT sceneIndex);

		static void SaveCurrent()
		{
			Get().SaveCurrentScene();
			Get().assets.Save();
		}

		static Scene* GetActiveScene() { return Get().activeScene.get(); }

        static float GetDT() { return Get().deltaTime.count(); };
        static float GetUpTime() { return Get().upTime.count(); };

		// Inputs
        static void SetKeyUp(UINT key)
        {
            keys[key] = false;
        }
        static void SetKeyDown(UINT key)
        {
            keys[key] = true;
        }
        static bool GetKeyDown(Keys key)
        {
            return keys[key];
        }


	private:
		Renderer renderer;
		Assets assets;

        std::chrono::duration<float> deltaTime;
        std::chrono::time_point<std::chrono::system_clock> frameStart;
		std::chrono::duration<float> upTime;

	private: // Scenes
		static const Okay::String SceneDecleration;
		std::unique_ptr<Scene> activeScene;

		static void ReadEntity(Entity& entity, std::ifstream& reader);
		static void ReadComponentData(Entity& entity, Components type, std::ifstream& reader);
		bool SaveCurrentScene(); 	


	private: // Inputs
		static bool keys[256];

	};

}