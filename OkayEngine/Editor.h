#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#include "Entity.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

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

		static std::weak_ptr<Mesh> pMesh;
		static std::weak_ptr<Material> pMaterial;
		static std::weak_ptr<Texture> pTexture;
		static Entity currentEntity;
		
		enum struct AssetType { INVALID, MESH, MATERIAL, TEXTURE, ENTITY };
		static void UpdateSelection(AssetType excludeType)
		{
			switch (excludeType)
			{
			default:
				return;
			case AssetType::MESH:
				pMaterial.reset();
				pTexture.reset();
				currentEntity.SetInvalid();
				return;

			case AssetType::MATERIAL:
				pMesh.reset();
				pTexture.reset();
				currentEntity.SetInvalid();
				return;

			case AssetType::TEXTURE:
				pMesh.reset();
				pMaterial.reset();
				currentEntity.SetInvalid();
				return;

			case AssetType::ENTITY:
				pMesh.reset();
				pMaterial.reset();
				pTexture.reset();
				return;
			}
		}

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


	private: // helpful variables
		static Okay::String newName;

	};
}