#define _CRT_SECURE_NO_WARNINGS

#include "Editor.h"
#include "Engine.h"

namespace Okay
{
	int Editor::index = -1;

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

	bool Editor::Update()
	{
		static bool dockSpace = true;

		if (ImGui::Begin("Temp selector"))
		{
			auto& reg = Engine::GetActiveScene()->GetRegistry();
			const int numAlive = (int)reg.alive();

			ImGui::InputInt("Index: ", &index);
			index = index >= numAlive ? numAlive - 1 : index < 0 ? 0 : index;
			
			index = numAlive ? index : -1;
		}
		ImGui::End();


		if (dockSpace)
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		if (ImGui::Begin("Dockspace"))
			ImGui::Checkbox("Enable Dockspace", &dockSpace);
		ImGui::End();

		DisplayEntityList();

		DisplayInspector();

		DisplayContent();

		//ImGuiWindowFlags_NoMove
		//ImGuiWindowFlags_NoBackground
		//ImGuiWindowFlags_NoTitleBar

		ImVec2 size(1600.f, 900.f);
		// Declare Viewport window
		if (ImGui::Begin("Viewport", nullptr))
			size = ImGui::GetWindowSize();
		ImGui::End();

		DX11& dx11 = DX11::Get();
		if ((UINT)size.x != dx11.GetMainWidth() || (UINT)size.y != dx11.GetMainHeight())
		{
			dx11.ResizeMainBuffer((UINT)size.x, (UINT)size.y);
			return true;
		}

		return false;
	}

	void Editor::NewFrame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void Editor::EndFrame()
	{
		DX11& dx11 = DX11::Get();

		if (ImGui::Begin("Viewport"))
			ImGui::Image((void*)*dx11.GetMainSRV(), ImVec2((float)dx11.GetMainWidth(), (float)dx11.GetMainHeight() - 16.f));
		ImGui::End();

		dx11.GetDeviceContext()->OMSetRenderTargets(1, DX11::Get().GetBackBufferRTV(), nullptr);

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

	}

	void Editor::DisplayEntityList()
	{
		auto& reg = Engine::GetActiveScene()->GetRegistry();

		if (!ImGui::Begin("Entities"))
		{
			ImGui::End();
			return;
		}

		reg.each([&reg](auto entity) 
		{
			ImGui::Text(reg.get<Okay::CompTag>(entity).tag.c_str);
		});

		ImGui::End();
	}

	void Editor::DisplayInspector()
	{
		Scene* pScene = Engine::GetActiveScene();
		auto& reg = pScene->GetRegistry();

		const bool Begun = ImGui::Begin("Inspector");

		if (!Begun || index == -1)
		{
			ImGui::End();
			return;
		}

		Entity& entity = pScene->GetEntities().at(index);
		ImGuiID ID = ImGui::GetID("Inspector");
		
		ImVec2 size = ImGui::GetWindowSize();

		// Transform
		if (ImGui::BeginChildFrame(ID++, { size.x, 100.f }))
		{
			ImGui::Text("Transform Component");
			ImGui::Separator();
			auto& tra = entity.GetComponent<Okay::CompTransform>();

			ImGui::DragFloat3("Position", &tra.position.x, 0.01f);
			ImGui::DragFloat3("Rotation", &tra.rotation.x, 0.01f);
			ImGui::DragFloat3("Scale", &tra.scale.x, 0.01f);

			tra.CalcMatrix();
		}
		ImGui::EndChildFrame();
	

		// Mesh
		if (ImGui::BeginChildFrame(ID++, { size.x, 120.f }))
		{
			Assets& assets = Engine::GetAssets();
			CompMesh& mesh = entity.GetComponent<CompMesh>();

			ImGui::Text("Mesh Component");
			ImGui::Separator();
			// Mesh
			ImGui::Text("Mesh:");
			if (ImGui::BeginCombo("##", mesh.mesh->GetName()))
			{
				for (UINT i = 0; i < assets.GetNumMeshes(); i++)
				{
					auto& name = assets.GetMeshName(i);

					if (ImGui::Selectable(name))
						mesh.AssignMesh(name.c_str);
	
				}
				ImGui::EndCombo();
			}

			// Materials
			ImGui::Text("\nMaterial:");
			if (ImGui::BeginCombo("###", mesh.materials.at(0)->GetName()))
			{
				for (UINT i = 0; i < assets.GetNumMaterials(); i++)
				{
					auto& name = assets.GetMaterialName(i);

					if (ImGui::Selectable(name))
						mesh.AssignMaterial(0, name);
				}

				ImGui::EndCombo();
			}



		}
		ImGui::EndChildFrame();
		
		if (ImGui::BeginChildFrame(ID++, { size.x, 100.f }))
		{
			ImGui::Text("Component 0");
			ImGui::Text("Component 1");
			ImGui::Text("Component 2");
			ImGui::Text("Component 3");
		}
		ImGui::EndChildFrame();


		ImGui::End();
	}


	void Editor::DisplayContent()
	{
		if (!ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_MenuBar))
		{
			ImGui::End();
			return;
		}

		ImGui::BeginMenuBar();
		

		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::MenuItem("Import"))
				OpenFileExplorer();

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();

		ImGui::Text("Item 0");
		ImGui::Text("Item 1");
		ImGui::Text("Item 2");
		ImGui::Text("Item 3");
		
		ImGui::End();
	}

	void Editor::OpenFileExplorer()
	{
		const size_t MaxFileLength = 500;
		OPENFILENAME ofn{};

		wchar_t fileName[MaxFileLength]{};

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = GetHWindow();
		ofn.lpstrFile = fileName;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = MaxFileLength;
		ofn.lpstrFilter = L"All Files\0*.*\0";
		ofn.nFilterIndex = 1;

		GetOpenFileName(&ofn);

		char text[MaxFileLength]{};
		wcstombs(text, ofn.lpstrFile, MaxFileLength);
		printf(text);
	}

}