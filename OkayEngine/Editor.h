#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

namespace Okay
{
	// Acts like a private namespace
	class Editor
	{
	private:
		Editor() = delete;
		Editor(const Editor&) = delete;
		Editor(Editor&&) = delete;
		Editor& operator=(const Editor&) = delete;
	public:
		static bool Create();
		static void Destroy();

		static bool Update();

		static void NewFrame();
		static void EndFrame();

	private:

		static int index;
		static void ClampIndex();

		static void DisplayEntityList();
		static void DisplayInspector();
		
		static void DisplayContent();
		static void OpenFileExplorer();

		static bool OpenMenuWindow(const ImVec2& pos, const char* name, bool* openFlag = nullptr)
		{
			static const ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
			
			if (ImGui::Begin(name, nullptr, flags))
			{
				ImGui::SetWindowPos(pos);
				
				if (openFlag && !ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
					*openFlag = false;


				return true;
			}

			return false;
		}
	};
}