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
		static void Create();
		static void Destroy();

		static void Update();

		static void NewFrame();
		static void EndFrame();

	private:
	};
}