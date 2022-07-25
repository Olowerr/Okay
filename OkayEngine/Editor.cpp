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

		//return;
		if (ImGui::Begin("My fancy window"))
		{
			ImGui::Text("Hellooo");
			ImGui::Checkbox("Dockspace", &dockSpace);

			int asd = 0;
			ImGui::DragInt("inter", &asd);
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