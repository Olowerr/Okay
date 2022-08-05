#pragma once
#include <iostream>
#include <chrono>

#include "ResourceManager.h"
#include "Renderer.h"
#include "Entity.h"

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
		static void Update();

		static bool LoadScene(const Okay::String& sceneName);
		static bool LoadScene(UINT sceneIndex);

		static void SaveCurrent()
		{
			Get().SaveCurrentScene();
			Get().assets.Save();
		}

		static Scene* GetActiveScene() { return Get().activeScene.get(); }

        static float GetDT() { return Get().deltaTime.count(); };

		// Inputs
		static void ReadInput(UINT message, WPARAM wParam, LPARAM lParam);
        static bool GetKeyDown(int key)
        {
            return Get().keys[key];
        }


	private:
		Renderer renderer;
		Assets assets;

        std::chrono::duration<float> deltaTime;
        std::chrono::time_point<std::chrono::system_clock> frameStart;

		template<typename T>
		static void foo(T* tPtr, std::ofstream& writer)
		{
			Okay::Components type = T::ID;
			writer.write((const char*)&type, sizeof(Okay::Components));
			tPtr->WritePrivateData(writer);
		}

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

enum Keys : unsigned char
{
    None = 0,

    Enter = 0xd,

    Esc = 0x1b,
    Space = 0x20,
   
    Left = 0x25,
    Up = 0x26,
    Right = 0x27,
    Down = 0x28,

    A = 0x41,
    B = 0x42,
    C = 0x43,
    D = 0x44,
    E = 0x45,
    F = 0x46,
    G = 0x47,
    H = 0x48,
    I = 0x49,
    J = 0x4a,
    K = 0x4b,
    L = 0x4c,
    M = 0x4d,
    N = 0x4e,
    O = 0x4f,
    P = 0x50,
    Q = 0x51,
    R = 0x52,
    S = 0x53,
    T = 0x54,
    U = 0x55,
    V = 0x56,
    W = 0x57,
    X = 0x58,
    Y = 0x59,
    Z = 0x5a,

    NumPad0 = 0x60,
    NumPad1 = 0x61,
    NumPad2 = 0x62,
    NumPad3 = 0x63,
    NumPad4 = 0x64,
    NumPad5 = 0x65,
    NumPad6 = 0x66,
    NumPad7 = 0x67,
    NumPad8 = 0x68,
    NumPad9 = 0x69,

    F1 = 0x70,
    F2 = 0x71,
    F3 = 0x72,
    F4 = 0x73,
    F5 = 0x74,
    F6 = 0x75,
    F7 = 0x76,
    F8 = 0x77,
    F9 = 0x78,
    F10 = 0x79,
    F11 = 0x7a,
    F12 = 0x7b,
    F13 = 0x7c,
    F14 = 0x7d,
    F15 = 0x7e,
    F16 = 0x7f,
    F17 = 0x80,
    F18 = 0x81,
    F19 = 0x82,
    F20 = 0x83,
    F21 = 0x84,
    F22 = 0x85,
    F23 = 0x86,
    F24 = 0x87,

    LeftShift = 0xa0,
    RightShift = 0xa1,
    LeftControl = 0xa2,
    RightControl = 0xa3,
    LeftAlt = 0xa4,
    RightAlt = 0xa5
};