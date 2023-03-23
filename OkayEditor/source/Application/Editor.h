#pragma once
#include <Engine/Application/Application.h>
#include <Engine/Application/Scene.h>
#include <Engine/Application/Entity.h>

#include <Engine/DirectX/DX11.h>
#include <Engine/Algorithms/Noise/PerlinNoise2D.h>

#include "imgui/imgui.h"

// Editor Entities should not be visable for the user
using EditorEntity = char;

class Editor : public Okay::Application
{
public:
	Editor(std::string_view startScene);
	~Editor();

	// Inherited via Application
	virtual void update() override;
	virtual void postRender() override;

private:
	Okay::Entity editorCamera;
	Okay::ContentBrowser& content;
	Okay::RenderTexture gameTexture;

	enum struct SelectionType { None, Entity, Mesh, Texture, Material, Shader};
	SelectionType selectionType;
	uint32_t selectionID;

	void newImGuiFrame();
	void endImGuiFrame();

	void displayEntities();
	inline Okay::Entity getEntity(uint32_t id);

	void displayInspector();
	void addComponents(Okay::Entity entity);
	void displayComponents(Okay::Entity entity);

	void displayMesh(uint32_t index);
	void displayTexture(uint32_t index);
	void displayMaterial(uint32_t index);
	void displayShader(uint32_t index);

	void displayContent();
	void displayAssetList();

	void setStyle();
	void displayStyling();

	void displaySceneSettings();

	// List label can be disabled by starting it with "##"
	template<typename T, typename... Args>
	bool selectTexture(T& instance, uint32_t selectedID, void (T::* function)(uint32_t), const char* listLabel);

	// List label can be disabled by starting it with "##"
	template<typename T>
	bool selectAsset(uint32_t selectedShaderID, uint32_t& result, const std::vector<T>& list, const char* listLabel);

	template<typename T, typename ResetFunction>
	Okay::Entity selectEntity(const char* label, uint32_t selectedEntity, ResetFunction resetFunction);
};

inline Okay::Entity Editor::getEntity(uint32_t id)
{
	return Okay::Entity((entt::entity)id, &scene.getRegistry());
}

template<typename T, typename... Args>
bool Editor::selectTexture(T& instance, uint32_t selectedTexID, void (T::* pFunction)(uint32_t), const char* listLabel)
{
	bool pressed = false;
	uint32_t idx = 0u;
	auto lambdaSelectTex = [&](const Okay::Texture& tex)
	{
		if (ImGui::Selectable(tex.getName().c_str(), idx == selectedTexID))
		{
			(instance.*pFunction)(idx);
			pressed = true;
		}
		ImGui::SameLine();
		ImGui::Image(tex.getSRV(), { ImVec2(15.f, 15.f) });
		idx++;
	};
	
	ID3D11ShaderResourceView* previewImg = selectedTexID != Okay::INVALID_UINT ? 
		content.getAsset<Okay::Texture>(selectedTexID).getSRV() : nullptr;

	ImGui::Image(previewImg, ImVec2(15.f, 15.f));
	ImGui::SameLine();
	if (ImGui::BeginCombo(listLabel, previewImg ? content.getAsset<Okay::Texture>(selectedTexID).getName().c_str() : nullptr))
	{
		if (ImGui::Selectable("Reset"))
		{
			(instance.*pFunction)(Okay::INVALID_UINT);
			pressed = true;
		}
		else
		{
			content.forEachAsset<Okay::Texture>(lambdaSelectTex);
		}

		ImGui::EndCombo();
	}

	return pressed;
}


template<typename T>
bool Editor::selectAsset(uint32_t currentSelectionId, uint32_t& result, const std::vector<T>& list, const char* listLabel)
{
	bool pressed = false;

	if (ImGui::BeginCombo(listLabel, list[currentSelectionId].getName().c_str()))
	{
		for (size_t i = 0; i < list.size(); i++)
		{
			if (ImGui::Selectable(list[i].getName().c_str(), i == currentSelectionId))
			{
				result = (uint32_t)i;
				pressed = true;
			}
		}

		ImGui::EndCombo();
	}

	return pressed;
}

template<typename T, typename OnResetFunction>
inline Okay::Entity Editor::selectEntity(const char* label, uint32_t currentEntity, OnResetFunction resetFunction)
{
	Okay::Entity selectedEntity;
	if (ImGui::BeginCombo(label, currentEntity == Okay::INVALID_UINT ? "None" : std::to_string(currentEntity).c_str()))
	{
		auto entityView = scene.getRegistry().view<T>(entt::exclude<EditorEntity>);

		if (ImGui::Selectable("Reset"))
			resetFunction();

		for (entt::entity entity : entityView)
		{
			if (ImGui::Selectable(std::to_string((uint32_t)entity).c_str(), (uint32_t)entity == currentEntity))
				selectedEntity = getEntity((uint32_t)entity);
		}

		ImGui::EndCombo();
	}

	return selectedEntity;
}
