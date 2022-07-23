#pragma once
#include "ResourceManager.h"
#include "Renderer.h"
#include "Entity.h"
#include <iostream>

// Temp
#define _IMGUI

#ifdef _IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#endif // _IMGUI


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

#ifdef _IMGUI
	private:
		void ImguiStart() 
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGui::StyleColorsDark();

			ImGui_ImplWin32_Init(GetHWindow());
			ImGui_ImplDX11_Init(DX11::Get().GetDevice(), DX11::Get().GetDeviceContext());
		}
		void ImguiEnd()
		{
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}

		void ImguiNewFrame()
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
		}
		void ImguiEndFrame()
		{
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

#endif // _IMGUI


	};

}

/*

	WRITE TEXTURES & MATERIALS TO FILE
	ENGINE -> ENTITY -> SCENE? VS ENGINE -> SCENE -> ENTITY
	WRITE TEXTURES & MATERIALS TO FILE
	ENGINE -> ENTITY -> SCENE? VS ENGINE -> SCENE -> ENTITY
	WRITE TEXTURES & MATERIALS TO FILE
	ENGINE -> ENTITY -> SCENE? VS ENGINE -> SCENE -> ENTITY
	WRITE TEXTURES & MATERIALS TO FILE
	ENGINE -> ENTITY -> SCENE? VS ENGINE -> SCENE -> ENTITY
	WRITE TEXTURES & MATERIALS TO FILE
	ENGINE -> ENTITY -> SCENE? VS ENGINE -> SCENE -> ENTITY
	WRITE TEXTURES & MATERIALS TO FILE
	ENGINE -> ENTITY -> SCENE? VS ENGINE -> SCENE -> ENTITY
	WRITE TEXTURES & MATERIALS TO FILE
	ENGINE -> ENTITY -> SCENE? VS ENGINE -> SCENE -> ENTITY

*/