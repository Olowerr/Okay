#include "Editor.h"
#include "Engine.h"

namespace Okay
{
	void Editor::Create()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplWin32_Init(GetHWindow());
		ImGui_ImplDX11_Init(DX11::Get().GetDevice(), DX11::Get().GetDeviceContext());
	}

	void Editor::Destroy()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void Editor::Update()
	{
		static bool dockSpace = false;
		
		if (dockSpace)
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		if (ImGui::Begin("Dockspace"))
			ImGui::Checkbox("Enable Dockspace", &dockSpace);
		ImGui::End();

		if (ImGui::Begin("Viewport"))
		{
			ImGui::Image((void*)DX11::Get().GetMainSRV(), ImGui::GetWindowSize());
		}
		ImGui::End();

		if (ImGui::Begin("Entitis"))
		{
			ImGui::Text("Entity 0");
			ImGui::Text("Entity 1");
			ImGui::Text("Entity 2");
			ImGui::Text("Entity 3");
		}
		ImGui::End();

		if (ImGui::Begin("Content Browser"))
		{
			ImGui::Text("Item 0");
			ImGui::Text("Item 1");
			ImGui::Text("Item 2");
			ImGui::Text("Item 3");
		}
		ImGui::End();

		if (ImGui::Begin("Inspector"))
		{
			ImGui::Text("Component 0");
			ImGui::Text("Component 1");
			ImGui::Text("Component 2");
			ImGui::Text("Component 3");
		}
		ImGui::End();

	}

	void Editor::NewFrame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void Editor::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

	}
}