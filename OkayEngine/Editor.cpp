#define _CRT_SECURE_NO_WARNINGS

#include "Editor.h"
#include "Engine.h"

namespace Okay
{
	std::weak_ptr<Mesh>	Editor::pMesh;
	std::weak_ptr<Material> Editor::pMaterial;
	std::weak_ptr<Texture> Editor::pTexture;
	Entity Editor::currentEntity;

	Okay::String Editor::newName;

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

		currentEntity.SetInvalid();

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


		//ImGuiWindowFlags_NoMove
		//ImGuiWindowFlags_NoBackground
		//ImGuiWindowFlags_NoTitleBar

		if (dockSpace)
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		if (ImGui::Begin("Dockspace"))
			ImGui::Checkbox("Enable Dockspace", &dockSpace);
		ImGui::End();

		DisplayEntityList();

		DisplayInspector();

		DisplayContent();




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
		const ImVec2 Size(ImGui::GetWindowSize());

		ImGui::Text("Entities: ");

		if (ImGui::Button("Add"))
		{
			currentEntity = pScene->CreateEntity();
			UpdateSelection(AssetType::ENTITY);

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

		if (ImGui::BeginListBox("##", { ImGui::GetWindowSize().x, -1.f }))
		{

			int c = 0;
			
			auto entities = reg.view<CompTag>();
			
			for (auto entity : entities)
			{
				if (ImGui::Selectable(entities.get<Okay::CompTag>(entity).tag, entity == currentEntity))
				{
					currentEntity.Set(entity, Engine::GetActiveScene());
					pMaterial.reset();
					pMesh.reset();
					pTexture.reset();
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					entityMenu = true;
					listMenu = false;
					menuPos = ImGui::GetMousePos();
				}
			}

#if 0
			for (auto& entity : entities)
			{
				if (ImGui::Selectable(reg.get<Okay::CompTag>(entity).tag.c_str, index == c))
					index = c;

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					entityMenu = true;
					listMenu = false;
					menuPos = ImGui::GetMousePos();
				}

				c++;
			}
#endif
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
					static Okay::String name;

					if (ImGui::InputText("###", name, sizeof(Okay::String), ImGuiInputTextFlags_EnterReturnsTrue))
					{
						currentEntity.GetComponent<Okay::CompTag>().tag = name;
						name = "";
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

	void Editor::DisplayInspector()
	{
		const bool Begun = ImGui::Begin("Inspector");

		if (!Begun || !currentEntity.IsValid())
		{
			ImGui::End();
			return;
		}

		Scene* pScene = Engine::GetActiveScene();
		auto& reg = pScene->GetRegistry();
		ImGuiID ID = ImGui::GetID("Inspector");

		ImVec2 size = ImGui::GetWindowSize();

		// Transform
		if (ImGui::BeginChildFrame(ID++, { size.x, 100.f }))
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
			if (ImGui::BeginChildFrame(ID++, { size.x, 120.f }))
			{
				Assets& assets = Engine::GetAssets();
				CompMesh& mesh = currentEntity.GetComponent<CompMesh>();

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
				if (ImGui::BeginCombo("###", mesh.GetMaterial()->GetName()))
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
		static Assets& assets = Engine::GetAssets();
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



		// Materials
		static bool matMenu = false;
		ImGui::SameLine();
		if (ImGui::BeginListBox("##", { Size.x, 0.f }))
		{
			ImGui::Text("Materials:");
			ImGui::Separator();

			static auto selectMaterial = [](std::shared_ptr<Material> material)
			{
				if (ImGui::Selectable(material->GetName(), material == pMaterial.lock()))
				{
					pMaterial = material;
					UpdateSelection(AssetType::MATERIAL);
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					pMaterial = material;
					UpdateSelection(AssetType::MATERIAL);

					matMenu = true;
					menuPos = ImGui::GetMousePos();
				}
			};

			assets.ForEachMaterial(selectMaterial);

		}
		ImGui::EndListBox();


		// Textures
		static bool texMenu = false;
		ImGui::SameLine();
		if (ImGui::BeginListBox("###", { Size.x, 0.f }))
		{
			ImGui::Text("Textures:");
			ImGui::Separator();

			static auto selectTexture = [](std::shared_ptr<Texture> texture)
			{
				if (ImGui::Selectable(texture->GetName(), texture == pTexture.lock()))
				{
					pTexture = texture;
					UpdateSelection(AssetType::TEXTURE);
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					pTexture = texture;
					UpdateSelection(AssetType::TEXTURE);

					texMenu = true;
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
					if (ImGui::InputText("###", newName, sizeof(Okay::String), ImGuiInputTextFlags_EnterReturnsTrue))
					{
						assets.ChangeTextureName(pTexture, newName);
						newName = "";
					}

					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Remove"))
				{
					assets.RemoveTexture(pTexture);
				}
			}
			ImGui::End();
		}
	}


	/*
		LOOK THROUGH THE EDITOR, MAYBE HOLD MATERIAL* INSTEAD OF: THE REDUNTANT GETMATERIALNAME(INT) SHIT
		JUST REMOVE THE USE OF INDEX AND USE PTRS, INSPECTOR SHOULD KNOW IF INSPECTING ASSET OR ENTITY
		LOOK THROUGH THE EDITOR, MAYBE HOLD MATERIAL* INSTEAD OF: THE REDUNTANT GETMATERIALNAME(INT) SHIT
		JUST REMOVE THE USE OF INDEX AND USE PTRS, INSPECTOR SHOULD KNOW IF INSPECTING ASSET OR ENTITY
		LOOK THROUGH THE EDITOR, MAYBE HOLD MATERIAL* INSTEAD OF: THE REDUNTANT GETMATERIALNAME(INT) SHIT
		JUST REMOVE THE USE OF INDEX AND USE PTRS, INSPECTOR SHOULD KNOW IF INSPECTING ASSET OR ENTITY
		LOOK THROUGH THE EDITOR, MAYBE HOLD MATERIAL* INSTEAD OF: THE REDUNTANT GETMATERIALNAME(INT) SHIT
		JUST REMOVE THE USE OF INDEX AND USE PTRS, INSPECTOR SHOULD KNOW IF INSPECTING ASSET OR ENTITY
		LOOK THROUGH THE EDITOR, MAYBE HOLD MATERIAL* INSTEAD OF: THE REDUNTANT GETMATERIALNAME(INT) SHIT
		JUST REMOVE THE USE OF INDEX AND USE PTRS, INSPECTOR SHOULD KNOW IF INSPECTING ASSET OR ENTITY
		LOOK THROUGH THE EDITOR, MAYBE HOLD MATERIAL* INSTEAD OF: THE REDUNTANT GETMATERIALNAME(INT) SHIT
		JUST REMOVE THE USE OF INDEX AND USE PTRS, INSPECTOR SHOULD KNOW IF INSPECTING ASSET OR ENTITY
		LOOK THROUGH THE EDITOR, MAYBE HOLD MATERIAL* INSTEAD OF: THE REDUNTANT GETMATERIALNAME(INT) SHIT
		JUST REMOVE THE USE OF INDEX AND USE PTRS, INSPECTOR SHOULD KNOW IF INSPECTING ASSET OR ENTITY
		LOOK THROUGH THE EDITOR, MAYBE HOLD MATERIAL* INSTEAD OF: THE REDUNTANT GETMATERIALNAME(INT) SHIT
		JUST REMOVE THE USE OF INDEX AND USE PTRS, INSPECTOR SHOULD KNOW IF INSPECTING ASSET OR ENTITY
		LOOK THROUGH THE EDITOR, MAYBE HOLD MATERIAL* INSTEAD OF: THE REDUNTANT GETMATERIALNAME(INT) SHIT
		JUST REMOVE THE USE OF INDEX AND USE PTRS, INSPECTOR SHOULD KNOW IF INSPECTING ASSET OR ENTITY
		LOOK THROUGH THE EDITOR, MAYBE HOLD MATERIAL* INSTEAD OF: THE REDUNTANT GETMATERIALNAME(INT) SHIT
		JUST REMOVE THE USE OF INDEX AND USE PTRS, INSPECTOR SHOULD KNOW IF INSPECTING ASSET OR ENTITY
		LOOK THROUGH THE EDITOR, MAYBE HOLD MATERIAL* INSTEAD OF: THE REDUNTANT GETMATERIALNAME(INT) SHIT
		JUST REMOVE THE USE OF INDEX AND USE PTRS, INSPECTOR SHOULD KNOW IF INSPECTING ASSET OR ENTITY

	
	*/


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
		Engine::GetAssets().TryImport(text);
	}

}