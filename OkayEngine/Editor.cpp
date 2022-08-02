#define _CRT_SECURE_NO_WARNINGS

#include "Editor.h"

namespace Okay
{
	Editor* Editor::editor;

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

		editor = new Editor;
		editor->currentEntity.SetInvalid();

		return true;
	}

	void Editor::Destroy()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		delete editor;
	}

	bool Editor::Update()
	{
		static bool dockSpace = true;


		//ImGuiWindowFlags_NoMove
		//ImGuiWindowFlags_NoBackground
		//ImGuiWindowFlags_NoTitleBar

		if (dockSpace)
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		if (ImGui::Begin("Dockspace"))
			ImGui::Checkbox("Enable Dockspace", &dockSpace);
		ImGui::End();


		editor->DisplayEntityList();

		editor->DisplayInspector();

		editor->DisplayContent();


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
		ofn.Flags = OFN_NOCHANGEDIR;

		if (!GetOpenFileName(&ofn))
			return;

		char text[MaxFileLength]{};
		wcstombs(text, ofn.lpstrFile, MaxFileLength);
		assets.TryImport(text);
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
		const ImVec2 Size(ImGui::GetWindowSize());

		ImGui::Text("Entities:");

		if (ImGui::Button("Add"))
		{
			currentEntity = pScene->CreateEntity();
			type = AssetType::ENTITY;

			// TEMP
			currentEntity.AddComponent<Okay::CompMesh>("cube.OkayAsset");
		}

		ImGui::SameLine();

		if (ImGui::Button("Remove") && currentEntity.IsValid())
		{
			pScene->DestroyEntity(currentEntity);
			currentEntity.SetInvalid();
		}

		ImGui::Separator();


		static bool entityMenu = false, listMenu = false;
		static ImVec2 menuPos = ImVec2();


		// Entities
		if (ImGui::BeginListBox("##", { ImGui::GetWindowSize().x, -1.f }))
		{
			auto entities = reg.view<CompTag>();
			
			for (auto entity : entities)
			{
				if (ImGui::Selectable(entities.get<Okay::CompTag>(entity).tag, entity == currentEntity))
				{
					currentEntity.Set(entity, Engine::GetActiveScene());
					type = AssetType::ENTITY;
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					entityMenu = true;
					listMenu = false;
					menuPos = ImGui::GetMousePos();
					type = AssetType::ENTITY;
				}
			}

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !entityMenu && ImGui::IsWindowHovered())
			{
				listMenu = true;
				menuPos = ImGui::GetMousePos();
			}

			ImGui::EndListBox();
		}

		if (ImGui::IsKeyReleased(ImGuiKey_P))
			entityMenu = false;

		ImGui::End();


		if (entityMenu)
		{
			if (OpenMenuWindow(menuPos, "EntityOptions", &entityMenu))
			{
				ImGui::Text("Entity Options:");
				ImGui::Separator();


				if (ImGui::BeginMenu("Change name"))
				{
					if (ImGui::InputText("###", newName, sizeof(Okay::String), ImGuiInputTextFlags_EnterReturnsTrue))
					{
						currentEntity.GetComponent<Okay::CompTag>().tag = newName;
						newName = "";
					}

					ImGui::EndMenu();
				}

				ImGui::MenuItem("Entity Option 1");
				ImGui::MenuItem("Entity Option 2");
			}
			ImGui::End();


		}

		else if (listMenu)
		{
			if (OpenMenuWindow(menuPos, "ListOptions", &listMenu))
			{
				ImGui::Text("List Options:");
				ImGui::Separator();

				ImGui::MenuItem("List Option 0");
				ImGui::MenuItem("List Option 1");
				ImGui::MenuItem("List Option 2");
			}
			ImGui::End();
		}
	}

	void Editor::DisplayContent()
	{
		if (!ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_MenuBar))
		{
			ImGui::End();
			return;
		}

		ImGuiID ID = ImGui::GetID("Content Browser");
		static const ImVec2 Size(120.f, 120.f);
		static ImVec2 menuPos;

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

			static auto displayMesh = [](std::shared_ptr<Mesh> mesh)
			{
				ImGui::Text(mesh->GetName());
			};

			assets.ForEachMesh(displayMesh);

		}
		ImGui::EndChildFrame();


		// Flags
		static bool matMenu = false;
		static bool texMenu = false;

		// Materials
		ImGui::SameLine();
		if (ImGui::BeginListBox("##", { Size.x, 0.f }))
		{
			ImGui::Text("Materials:");
			ImGui::Separator();
			
			auto selectMaterial = [](std::shared_ptr<Material> material)
			{
				if (ImGui::Selectable(material->GetName(), material == editor->pMaterial.lock()))
				{
					editor->pMaterial = material;
					editor->type = AssetType::MATERIAL;
					editor->matDesc = editor->pMaterial.lock()->GetDesc();
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					editor->pMaterial = material;
					editor->type = AssetType::MATERIAL;
					editor->matDesc = editor->pMaterial.lock()->GetDesc();

					matMenu = true;
					texMenu = false;
					menuPos = ImGui::GetMousePos();
				}
			};

			assets.ForEachMaterial(selectMaterial);

		}
		ImGui::EndListBox();


		// Textures
		ImGui::SameLine();
		if (ImGui::BeginListBox("###", { Size.x, 0.f }))
		{
			ImGui::Text("Textures:");
			ImGui::Separator();

			static auto selectTexture = [](std::shared_ptr<Texture> texture)
			{
				if (ImGui::Selectable(texture->GetName(), texture == editor->pTexture.lock()))
				{
					editor->pTexture = texture;
					editor->type = AssetType::TEXTURE;
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					editor->pTexture = texture;
					editor->type = AssetType::TEXTURE;

					texMenu = true;
					matMenu = false;
					menuPos = ImGui::GetMousePos();
				}
			};

			assets.ForEachTexture(selectTexture);

		}
		ImGui::EndListBox();

		ImGui::End();


		// Menu Handling
		if (matMenu)
		{
			if (OpenMenuWindow(menuPos, "MatMenu", &matMenu))
			{
				ImGui::Text("Material Options");
				ImGui::Separator();

				if (ImGui::BeginMenu("Change name"))
				{
					if (ImGui::InputText("###", newName, sizeof(Okay::String), ImGuiInputTextFlags_EnterReturnsTrue))
					{
						assets.ChangeMaterialName(pMaterial, newName);
						newName = "";
					}

					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Remove"))
				{
					assets.RemoveMaterial(pMaterial);
				}
			}
			ImGui::End();
		}
		else if (texMenu)
		{
			if (OpenMenuWindow(menuPos, "TexMenu", &texMenu))
			{
				ImGui::Text("Texture Options");
				ImGui::Separator();

				if (ImGui::BeginMenu("Change name"))
				{
					/*if (ImGui::InputText("###", newName, sizeof(Okay::String), ImGuiInputTextFlags_EnterReturnsTrue))
					{
						assets.ChangeTextureName(pTexture, newName);
						newName = "";
					}*/

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Remove"))
					assets.RemoveTexture(pTexture);
				
			}
			ImGui::End();
		}
	}

	void Editor::DisplayInspector()
	{
		if (ImGui::Begin("Inspector"))
		{
			ImGuiID ID = ImGui::GetID("Inspector");
			const ImVec2 Size = ImGui::GetWindowSize();

			switch (type)
			{
			default:
				break;
			case AssetType::ENTITY:
				InspectComponents(ID, Size);
				break;
			case AssetType::MATERIAL:
				InspectMaterial(ID, Size);
				break;
			case AssetType::TEXTURE:
				InspectTexture(ID, Size);
				break;
			}
		}

		ImGui::End();
	}

	void Editor::InspectComponents(ImGuiID& id, const ImVec2& Size)
	{
		if (!currentEntity.IsValid())
			return;

		Scene* pScene = Engine::GetActiveScene();
		auto& reg = pScene->GetRegistry();
		

		// Transform
		if (ImGui::BeginChildFrame(id++, { Size.x, 100.f }))
		{
			ImGui::Text("Transform Component");
			ImGui::Separator();
			auto& tra = currentEntity.GetComponent<Okay::CompTransform>();

			ImGui::DragFloat3("Position", &tra.position.x, 0.01f);
			ImGui::DragFloat3("Rotation", &tra.rotation.x, 0.01f);
			ImGui::DragFloat3("Scale", &tra.scale.x, 0.01f);

			tra.CalcMatrix();
		}
		ImGui::EndChildFrame();


		// Mesh
		if (currentEntity.HasComponent<Okay::CompMesh>())
		{
			if (ImGui::BeginChildFrame(id++, { Size.x, 120.f }))
			{
				CompMesh& compMesh = currentEntity.GetComponent<CompMesh>();

				ImGui::Text("Mesh Component");
				ImGui::Separator();

				// Mesh
				ImGui::Text("Mesh:");
				if (ImGui::BeginCombo("##", compMesh.mesh->GetName()))
				{
					static auto ListMeshes = [&compMesh](std::shared_ptr<Mesh> mesh)
					{
						const String& name = mesh->GetName();

						if (ImGui::Selectable(name))
							compMesh.AssignMesh(mesh);
					};

					assets.ForEachMesh(ListMeshes);

					ImGui::EndCombo();
				}

				// Materials
				ImGui::Text("\nMaterial:");
				if (ImGui::BeginCombo("###", compMesh.GetMaterial()->GetName()))
				{
					static auto ListMaterials = [&compMesh](std::shared_ptr<Material> material)
					{
						const String& name = material->GetName();

						if (ImGui::Selectable(name))
							compMesh.AssignMaterial(0, name);
					};

					assets.ForEachMaterial(ListMaterials);

					ImGui::EndCombo();
				}
			}
			ImGui::EndChildFrame();
		}
	}

	void Editor::InspectMaterial(ImGuiID& id, const ImVec2& Size)
	{
		if (pMaterial.expired())
			return;

		ImGui::Text("Name: %s", matDesc.name);
		ImGui::Separator();
		
		static std::shared_ptr<Material> pMat;
		pMat = pMaterial.lock();

		if (ImGui::BeginChildFrame(id++, { Size.x, 120.f }))
		{
			ImGui::Text("Base Colour:"); ImGui::SameLine();
			ImGui::ImageButton((void*)*pMat->GetBaseColour()->GetSRV(), {15.f, 15.f}, {}, {1.f, 1.f}, 1);
			
			ImGui::Text("Specular Colour:"); ImGui::SameLine();
			ImGui::ImageButton((void*)*pMat->GetSpecular()->GetSRV(), {15.f, 15.f}, {}, {1.f, 1.f}, 1);
			
			ImGui::Text("Ambient Colour:"); ImGui::SameLine();
			ImGui::ImageButton((void*)*pMat->GetAmbient()->GetSRV(), {15.f, 15.f}, {}, {1.f, 1.f}, 1);

			ImGui::DragFloat2("UV Offset", &pMat->GetGPUData().uvOffset.x, 0.001f);
			ImGui::DragFloat2("UV Tiling", &pMat->GetGPUData().uvTiling.x, 0.001f);
		}
		ImGui::EndChildFrame();
	}

	void Editor::InspectTexture(ImGuiID& id, const ImVec2& Size)
	{
	}

}