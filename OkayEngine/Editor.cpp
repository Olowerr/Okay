#define _CRT_SECURE_NO_WARNINGS

#include "Editor.h"
#include "Engine.h"

namespace Okay
{
	int Editor::index = -1;

	bool Editor::Create()
	{
		IMGUI_CHECKVERSION();
		VERIFY(ImGui::CreateContext());

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

		VERIFY(ImGui_ImplWin32_Init(GetHWindow()));
		VERIFY(ImGui_ImplDX11_Init(DX11::Get().GetDevice(), DX11::Get().GetDeviceContext()));

		ClampIndex();

		return true;
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
		if (!ImGui::Begin("Entity List"))
		{
			ImGui::End();
			return;
		}

		Scene* pScene = Engine::GetActiveScene();
		auto& reg = pScene->GetRegistry();
		Entity entity = pScene->GetEntities().at(index);
		const ImVec2 Size(ImGui::GetWindowSize());

		ImGui::Text("Entities: ");

		if (ImGui::Button("Add"))
		{
			Entity ent = pScene->CreateEntity();
			index = (int)pScene->GetEntities().size() - 1;

			// TEMP
			ent.AddComponent<Okay::CompMesh>("cube.OkayAsset");
		}

		ImGui::SameLine();
		
		if (ImGui::Button("Remove"))
		{
			pScene->DestroyEntity(entity);
			ClampIndex();
		}

		ImGui::Separator();

		if (ImGui::BeginListBox("##"))
		{
			static bool clicked = false;
			bool frame = false;

			int c = 0;
			auto& entities = pScene->GetEntities();
			for (auto& entity : entities)
			{
				if (ImGui::Selectable(reg.get<Okay::CompTag>(entity).tag.c_str, index == c))
					index = c;

				c++;
			}


			ImGui::EndListBox();
		}

			

		ImGui::End();
	}

	void Editor::ClampIndex()
	{
		const int numAlive = (int)Engine::GetActiveScene()->GetRegistry().alive();
		if (!numAlive)
		{
			index = -1;
			return;
		}

		index = index >= numAlive ? numAlive - 1 : index < 0 ? 0 : index;
	}

	void Editor::DisplayInspector()
	{
		const bool Begun = ImGui::Begin("Inspector");

		if (!Begun || index == -1)
		{
			ImGui::End();
			return;
		}

		Scene* pScene = Engine::GetActiveScene();
		auto& reg = pScene->GetRegistry();
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
		if (entity.HasComponent<Okay::CompMesh>())
		{
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
		}
		
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

		ImGuiID ID = ImGui::GetID("Content Browser");
		Assets& assets = Engine::GetAssets();
		const ImVec2 Size(120.f, 120.f);

		ImGui::BeginMenuBar();
		

		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::MenuItem("Import"))
				OpenFileExplorer();

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();

		
		// Meshes
		if (ImGui::BeginChildFrame(ID++, Size))
		{
			ImGui::Text("Meshes:");
			ImGui::Separator();

			for (UINT i = 0; i < assets.GetNumMeshes(); i++)
			{
				ImGui::Text(assets.GetMeshName(i));
			}

		}
		ImGui::EndChildFrame();



		// Materials
		ImGui::SameLine();
		if (ImGui::BeginChildFrame(ID++, Size))
		{
			ImGui::Text("Materials:");
			ImGui::Separator();

			for (UINT i = 0; i < assets.GetNumMaterials(); i++)
			{
				ImGui::Text(assets.GetMaterialName(i));
			}

		}
		ImGui::EndChildFrame();


		// Textures
		ImGui::SameLine();
		if (ImGui::BeginChildFrame(ID++, { Size.x + 40.f, Size.y }))
		{
			ImGui::Text("Textures:");
			ImGui::Separator();
			for (UINT i = 0; i < assets.GetNumTextures(); i++)
			{
				ImGui::Text(assets.GetTextureName(i));
			}

		}
		ImGui::EndChildFrame();
		


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
		ofn.lpstrFilter = L"All Files\0*.*\0Other Files\0*.png\0";
		ofn.nFilterIndex = 1;

		GetOpenFileName(&ofn);

		char text[MaxFileLength]{};
		wcstombs(text, ofn.lpstrFile, MaxFileLength);
		printf(text);
	}

}