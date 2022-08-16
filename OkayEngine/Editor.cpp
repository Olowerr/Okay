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

	void Editor::Update()
	{
		static bool dockSpace = true;


		//ImGuiWindowFlags_NoMove
		//ImGuiWindowFlags_NoBackground
		//ImGuiWindowFlags_NoTitleBar

		if (dockSpace)
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		if (ImGui::Begin("Dockspace"))
		{
			ImGui::BeginDisabled();
			ImGui::Checkbox("Enable Dockspace", &dockSpace);
			ImGui::EndDisabled();
		}
		ImGui::End();

		editor->DisplayEntityList();
		editor->DisplayInspector();
		editor->DisplayContent();

		static ImVec2 size(WIN_W, WIN_H);

		// Declare Viewport window
		if (ImGui::Begin("Viewport", nullptr))
			size = ImGui::GetWindowSize();
		ImGui::End();

		DX11& dx11 = DX11::Get();
		if ((UINT)size.x != dx11.GetMainWidth() || (UINT)size.y != dx11.GetMainHeight())
		{
			dx11.ResizeMainBuffer((UINT)size.x, (UINT)size.y);
			Engine::GetRenderer().Resize();
		}
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
			UpdateSelection(AssetType::ENTITY);
		}

		ImGui::SameLine();

		if (ImGui::Button("Remove") && currentEntity.IsValid())
		{
			pScene->DestroyEntity(currentEntity);
			UpdateSelection(AssetType::NONE);
		}

		ImGui::Separator();


		static bool entityMenu = false, listMenu = false;
		static ImVec2 menuPos = ImVec2();


		// Entities
		if (ImGui::BeginListBox("##EntNoLabel", { ImGui::GetWindowSize().x, -1.f }))
		{
			auto entities = reg.view<CompTag>();
			
			for (auto entity : entities)
			{
				if (ImGui::Selectable(entities.get<CompTag>(entity).tag, entity == currentEntity))
				{
					currentEntity = Entity(entity, Engine::GetActiveScene());
					UpdateSelection(AssetType::ENTITY);
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					currentEntity = Entity(entity, Engine::GetActiveScene());
					UpdateSelection(AssetType::ENTITY);

					entityMenu = true;
					listMenu = false;
					menuPos = ImGui::GetMousePos();
				}
			}

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !entityMenu && ImGui::IsWindowHovered())
			{
				listMenu = true;
				menuPos = ImGui::GetMousePos();
			}

			ImGui::EndListBox();
		}

		ImGui::End();


		if (entityMenu)
		{
			if (OpenMenuWindow(menuPos, "EntityOptions", &entityMenu))
			{
				ImGui::Text("Entity Options:");
				ImGui::Separator();


				if (ImGui::BeginMenu("Change name"))
				{
					if (ImGui::InputText("###", newName, sizeof(String), ImGuiInputTextFlags_EnterReturnsTrue))
					{
						currentEntity.GetComponent<CompTag>().tag = newName;
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
		static const ImVec2 Size(120.f, 0.f);
		static ImVec2 menuPos;
		
		// Flags
		static bool meshMenu = false;
		static bool matMenu = false;
		static bool texMenu = false;

		ImGui::BeginMenuBar();

		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::MenuItem("Import"))
				OpenFileExplorer();

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();


		// Meshes
		if (ImGui::BeginListBox("##LMeshLabel", Size))
		{
			ImGui::Text("Meshes:");
			ImGui::Separator();

			static auto displayMesh = [](const SPtr<Mesh>& mesh)
			{
				if (ImGui::Selectable(mesh->GetName(), mesh == editor->pMesh.lock()))
				{
					editor->pMesh = mesh;
					editor->UpdateSelection(AssetType::MESH);
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					editor->pMesh = mesh;
					editor->UpdateSelection(AssetType::MESH);

					meshMenu = true;
					matMenu = false;
					texMenu = false;
					menuPos = ImGui::GetMousePos();
				}
			};

			assets.ForEachMesh(displayMesh);

		}
		ImGui::EndListBox();



		// Materials
		ImGui::SameLine();
		if (ImGui::BeginListBox("##LMatLabel", Size))
		{
			ImGui::Text("Materials:");
			ImGui::Separator();
			
			static auto selectMaterial = [](const SPtr<Material>& material)
			{
				if (ImGui::Selectable(material->GetName(), material == editor->pMaterial.lock()))
				{
					editor->pMaterial = material;
					editor->UpdateSelection(AssetType::MATERIAL);
					editor->matDesc = editor->pMaterial.lock()->GetDesc();
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					editor->pMaterial = material;
					editor->UpdateSelection(AssetType::MATERIAL);
					editor->matDesc = editor->pMaterial.lock()->GetDesc();

					matMenu = true;
					meshMenu = false;
					texMenu = false;
					menuPos = ImGui::GetMousePos();
				}
			};

			assets.ForEachMaterial(selectMaterial);

		}
		ImGui::EndListBox();


		// Textures
		ImGui::SameLine();
		if (ImGui::BeginListBox("##LTexLabel", Size))
		{
			ImGui::Text("Textures:");
			ImGui::Separator();

			static auto selectTexture = [](const SPtr<Texture>& texture)
			{
				if (ImGui::Selectable(texture->GetName(), texture == editor->pTexture.lock()))
				{
					editor->pTexture = texture;
					editor->UpdateSelection(AssetType::TEXTURE);
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					editor->pTexture = texture;
					editor->UpdateSelection(AssetType::TEXTURE);

					texMenu = true;
					meshMenu = false;
					matMenu = false;
					menuPos = ImGui::GetMousePos();
				}
			};

			assets.ForEachTexture(selectTexture);

		}
		ImGui::EndListBox();

		ImGui::End();


		// Menu Handling
		if (meshMenu)
		{
			if (OpenMenuWindow(menuPos, "MeshMenu", &meshMenu))
			{
				ImGui::Text("Mesh Options");
				ImGui::Separator();

				if (ImGui::BeginMenu("Change name"))
				{
					/*if (ImGui::InputText("##inputName", newName, sizeof(String), ImGuiInputTextFlags_EnterReturnsTrue))
					{
						//assets.ChangeMeshName(pMesh, newName);
						newName = "";
						meshMenu = false;
					}*/

					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Remove"))
				{
					assets.RemoveMesh(pMesh);
					UpdateSelection(AssetType::NONE);
					meshMenu = false;
				}		
			}
			ImGui::End();
		}
		else if (matMenu)
		{
			if (OpenMenuWindow(menuPos, "MatMenu", &matMenu))
			{
				ImGui::Text("Material Options");
				ImGui::Separator();

				if (ImGui::BeginMenu("Change name"))
				{
					if (ImGui::InputText("##inputName", newName, sizeof(String), ImGuiInputTextFlags_EnterReturnsTrue))
					{
						assets.ChangeMaterialName(pMaterial, newName);
						newName = "";
						matMenu = false;
					}

					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Remove"))
				{
					assets.RemoveMaterial(pMaterial);
					UpdateSelection(AssetType::NONE);
					matMenu = false;
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
					/*if (ImGui::InputText("###", newName, sizeof(String), ImGuiInputTextFlags_EnterReturnsTrue))
					{
						assets.ChangeTextureName(pTexture, newName);
						newName = "";
						texMenu = false;
					}*/

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Remove"))
				{
					assets.RemoveTexture(pTexture);
					UpdateSelection(AssetType::NONE);
					texMenu = false;
				}
				
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
		static ImVec2 mousePos;

		ImGui::Text("Entity: %s", currentEntity.GetComponent<CompTag>().tag);
		
		//ImGui::SameLine();
		ImGui::PushItemWidth(-1.f);
		if (ImGui::BeginCombo("##AddComp", "Add Component"))
		{
			if (ImGui::Selectable("Mesh"))
			{
				if (!currentEntity.HasComponent<CompMesh>())
					currentEntity.AddComponent<CompMesh>();
			}
			
			else if (ImGui::Selectable("Point Light"))
			{
				if (!currentEntity.HasComponent<CompPointLight>())
					currentEntity.AddComponent<CompPointLight>();
			}
			

			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("##RemComp", "Remove Component"))
		{
			if (ImGui::Selectable("Mesh"))
			{
				if (currentEntity.HasComponent<CompMesh>())
					currentEntity.RemoveComponent<CompMesh>();
			}
			
			else if (ImGui::Selectable("Point Light"))
			{
				if (currentEntity.HasComponent<CompPointLight>())
					currentEntity.RemoveComponent<CompPointLight>();
			}
			

			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		ImGui::Separator();


		// Transform
		if (ImGui::BeginChildFrame(id++, { Size.x, 100.f }))
		{
			ImGui::Text("Transform Component");
			ImGui::Separator();
			auto& tra = currentEntity.GetComponent<CompTransform>();

			ImGui::PushItemWidth(-15.f);

			ImGui::Text("Position:"); ImGui::SameLine();
			ImGui::DragFloat3("##PosNoLabel", &tra.position.x, 0.01f);

			ImGui::Text("Rotation:"); ImGui::SameLine();
			ImGui::DragFloat3("##RotNoLabel", &tra.rotation.x, 0.01f);

			ImGui::Text("Scale:   "); ImGui::SameLine();
			ImGui::DragFloat3("##ScaNoLabel", &tra.scale.x, 0.01f);

			ImGui::PopItemWidth();

			tra.CalcMatrix();

		}
		ImGui::EndChildFrame();


		// Mesh
		if (currentEntity.HasComponent<CompMesh>())
		{
			if (ImGui::BeginChildFrame(id++, { Size.x, 120.f }))
			{
				CompMesh& compMesh = currentEntity.GetComponent<CompMesh>();

				ImGui::Text("Mesh Component");
				ImGui::Separator();

				// Mesh
				ImGui::Text("Mesh:");
				ImGui::PushItemWidth(-15.f);
				if (ImGui::BeginCombo("##MeshNoLabel", compMesh.GetMesh()->GetName()))
				{
					static auto ListMeshes = [](const SPtr<const Mesh>& mesh, CompMesh& cMesh)
					{
						if (ImGui::Selectable(mesh->GetName()))
							cMesh.AssignMesh(mesh);
					};

					assets.ForEachMesh(ListMeshes, compMesh);

					ImGui::EndCombo();
				}

				// Materials
				ImGui::Text("Material:");
				if (ImGui::BeginCombo("##MatNoLabel", compMesh.GetMaterial()->GetName()))
				{
					static auto ListMaterials = [](const SPtr<const Material>& material, CompMesh& cMesh)
					{
						if (ImGui::Selectable(material->GetName()))
							cMesh.AssignMaterial(0, material);
					};

					assets.ForEachMaterial(ListMaterials, compMesh);

					ImGui::EndCombo();
				}

			}
			ImGui::PopItemWidth();
			ImGui::EndChildFrame();
		}

		if (currentEntity.HasComponent<CompPointLight>())
		{
			if (ImGui::BeginChildFrame(id++, { Size.x, 120.f }))
			{
				CompPointLight& light = currentEntity.GetComponent<CompPointLight>();


				ImGui::Text("Point Light");
				ImGui::Separator();

				ImGui::PushItemWidth(-15.f);

				ImGui::Text("Intensity:"); ImGui::SameLine();
				ImGui::DragFloat("##LightIntLabel", &light.intensity, 0.01f, 0.f, 100.f, nullptr, ImGuiSliderFlags_Logarithmic);

				ImGui::Text("Attenuation:"); ImGui::SameLine();
				ImGui::DragFloat2("##LightAttLabel", &light.attenuation.x, 0.001f, 0.f, 1.f, nullptr, ImGuiSliderFlags_Logarithmic);
				
				ImGui::Text("Colour:"); //ImGui::SameLine();
				ImGui::ColorEdit3("##LightColLabel", &light.colour.x);

				ImGui::PopItemWidth();

			}
			ImGui::EndChildFrame();
		}
	}

	void Editor::InspectMaterial(ImGuiID& id, const ImVec2& Size)
	{
		if (pMaterial.expired())
			return;

		ImGui::Text("Material: %s", matDesc.name);
		ImGui::Separator();
		
		static const ImVec2 ImageSize(15.f, 15.f);

		static SPtr<Material> pMat;
		static SPtr<const Texture> pBase, pSpecular, pAmbient;

		pMat = pMaterial.lock();
		pBase = pMat->GetBaseColour();
		pSpecular = pMat->GetSpecular();
		pAmbient = pMat->GetAmbient();

		static SPtr<Texture> chosenTexture;
		chosenTexture = nullptr;

		static auto selectTextures = [](const SPtr<Texture>& texture, const SPtr<const Texture>& matTexture, String& descTexName)
		{
			if (ImGui::Selectable(texture->GetName(), texture == matTexture))
			{
				chosenTexture = texture;
				descTexName = texture->GetName();
			}

			ImGui::SameLine();
			ImGui::Image(*texture->GetSRV(), ImageSize);
		};

		if (ImGui::BeginChildFrame(id++, { -1.f, 220.f }))
		{
			ImGui::PushItemWidth(-1.f);
			ImGui::Text("Base Colour:"); ImGui::SameLine();
			ImGui::Image(*pBase->GetSRV(), ImageSize);

			if (ImGui::BeginCombo("##NoBaseLabel", matDesc.baseColour))
			{
				assets.ForEachTexture(selectTextures, pBase, matDesc.baseColour);

				if (chosenTexture)
					pMat->SetBaseColour(chosenTexture);

				ImGui::EndCombo();
			}

			ImGui::Text("Specular:"); ImGui::SameLine();
			ImGui::Image(*pSpecular->GetSRV(), ImageSize);

			
			if (ImGui::BeginCombo("##NoSpecLabel",matDesc.specular))
			{
				assets.ForEachTexture(selectTextures, pSpecular, matDesc.specular);

				if (chosenTexture)
					pMat->SetSpecular(chosenTexture);

				ImGui::EndCombo();
			}

			ImGui::Text("Ambient:"); ImGui::SameLine();
			ImGui::Image(*pAmbient->GetSRV(), ImageSize);

			if (ImGui::BeginCombo("##NoAmbiLabel", matDesc.ambient))
			{
				assets.ForEachTexture(selectTextures, pAmbient, matDesc.ambient);

				if (chosenTexture)
					pMat->SetAmbient(chosenTexture);

				ImGui::EndCombo();
			}

			ImGui::Separator();

			ImGui::Text("UV Offset:");
			ImGui::DragFloat2("##NoOffsetLabel", &pMat->GetGPUData().uvOffset.x, 0.002f);

			ImGui::Text("UV Tiling:");
			ImGui::DragFloat2("##NoTileLabel", &pMat->GetGPUData().uvTiling.x, 0.002f);

			ImGui::PopItemWidth();
		}
		ImGui::EndChildFrame();

		pMat.reset(); 
		pBase.reset(); 
		pSpecular.reset(); 
		pAmbient.reset(); 

	}

	void Editor::InspectTexture(ImGuiID& id, const ImVec2& Size)
	{
		if (pTexture.expired())
			return;

		static SPtr<const Texture> pTex;
		pTex = pTexture.lock();

		static ImVec2 imgSize;
		imgSize.x = Size.x - 25.f;
		imgSize.y = imgSize.x / pTex->GetAspectRatio();

		ImGui::Text("Texture: %s", pTex->GetName());
		ImGui::Separator();

		if (ImGui::BeginChildFrame(id++, {0.f, imgSize.y + 40.f}))
		{
			ImGui::Text("Width: %d\nHeight: %d", pTex->GetWidth(), pTex->GetHeight());

			ImGui::Image(*pTexture.lock()->GetSRV(), imgSize);
		}
		ImGui::EndChildFrame();

		pTex.reset();
	}

}