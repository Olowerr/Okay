#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#include "Engine/Engine.h"

namespace Okay
{
	// Acts like a private namespace
	class Editor
	{
	private:
		Editor() = default;
		~Editor() = default;
		Editor(const Editor&) = delete;
		Editor(Editor&&) = delete;
		Editor& operator=(const Editor&) = delete;

	public:
		static bool Create();
		static void Destroy();

		static void Update();

		static void NewFrame();
		static void EndFrame();

	private:
		template <typename T>
		using SPtr = std::shared_ptr<T>;

		// std::unique_ptr requires the constructor and destructor to be public
		// which is to be avoided.
		static Editor* editor;


		std::weak_ptr<Mesh> pMesh;
		std::weak_ptr<Material> pMaterial;
		std::weak_ptr<Texture> pTexture;
		Entity currentEntity;

		enum struct AssetType { NONE, MESH, MATERIAL, TEXTURE, ENTITY };
		AssetType type = AssetType::NONE;

		void UpdateSelection(AssetType excludeType)
		{
			type = excludeType;
			newName = "";
			switch (excludeType)
			{
			default:
				pMesh.reset();
				pMaterial.reset();
				pTexture.reset();
				currentEntity.SetInvalid();
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

		void DisplayEntityList();
		void DisplayContent();
		void DisplayInspector();

		bool OpenMenuWindow(const ImVec2& pos, const char* name, bool* openFlag = nullptr)
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
		void OpenFileExplorer();

	private: // helpful variables
		Okay::String newName;
		Assets& assets = Engine::GetAssets();
		Okay::MaterialDesc_Strs matDesc;


	private: // Inspect Helper Functions
		void InspectComponents(ImGuiID& id, const ImVec2& Size);
		void InspectMaterial(ImGuiID& id, const ImVec2& Size);
		void InspectTexture(ImGuiID& id, const ImVec2& Size);
	};
}

