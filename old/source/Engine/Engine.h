#pragma once
#include <iostream>
#include <chrono>

#include "Graphics/Importing/ResourceManager.h"
#include "Renderer.h"
#include "ECS/Entity.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

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

		// Get Inputs
        static bool GetKeyDown(int key)
        {
            return keysDown[key];
        }
		static bool GetKeyRelease(int key)
        {
            return keysDown[key];
        }

		static float GetMouseDeltaX() { return (float)Get().lastState.lX; }
		static float GetMouseDeltaY() { return (float)Get().lastState.lY; }
		static bool GetMouseLocked() { return Get().mouseLocked; }

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
		static bool keysDown[256];
		static bool keysReleased[256];

		LPDIRECTINPUT8 dInput;
		IDirectInputDevice8* DIMouse;
		IDirectInputDevice8* DIKeyboard;
		DIMOUSESTATE lastState;

		bool mouseLocked = false;


	};

}




#define COUNT_COMP(T) numComp += (bool)registry.try_get<Okay::T>(entity);

#define WRITE_DATA(T)												\
if (auto* tPtr = registry.try_get<Okay::T>(entity))					\
{																	\
	Okay::Components type = Okay::T::ID;							\
	writer.write((const char*)&type, sizeof(Okay::Components));		\
	tPtr->WritePrivateData(writer);									\
}