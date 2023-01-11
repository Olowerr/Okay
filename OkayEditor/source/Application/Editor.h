#pragma once
#include <Engine/Application/Application.h>
#include <Engine/Application/Scene.h>
#include <Engine/DirectX/DX11.h>
#include <Engine/Application/Entity.h>
#include <Engine/Graphics/Noise/PerlinNoise2D.h>

#include "imgui/imgui.h"

class Editor : public Application
{
public:
	Editor(std::string_view startScene);
	~Editor();

	// Inherited via Application
	virtual void run() override;

private:
	Okay::Scene scene;
	Okay::RenderTexture gameTexture;

	void newFrame();
	void update();
	void endFrame();

	enum struct SelectionType { None, Entity, Mesh, Texture, Material, Shader};
	SelectionType selectionType;
	uint32_t selectionID;

	void displayEntities();

	void displayInspector();
	void addComponents(Okay::Entity entity);
	void displayComponents(Okay::Entity entity);
	uint32_t XIconID;

	void displayMesh(uint32_t index);
	void displayTexture(uint32_t index);
	void displayMaterial(uint32_t index);
	void displayShader(uint32_t index);

	void displayContent();
	void displayAssetList();
	void openFileExplorer();

	Okay::RenderTexture* testTex;
	Okay::PerlinNoise2D* noiser;

	// List label can be disabled by starting it with "##"
	template<typename T, typename... Args>
	bool selectTexture(T& instance, uint32_t selectedID, void (T::* function)(uint32_t), const char* listLabel);
};


template<typename T, typename... Args>
bool Editor::selectTexture(T& instance, uint32_t selectedTexID, void (T::* pFunction)(uint32_t), const char* listLabel)
{
	bool pressed = false;
	uint32_t idx = 0u;
	static auto lambdaSelectTex = [&](const Okay::Texture& tex)
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

	ID3D11ShaderResourceView* previewImg = selectedTexID != Okay::INVALID_UINT ? content.getTexture(selectedTexID).getSRV() : nullptr;
	ImGui::Image(previewImg, { ImVec2(15.f, 15.f) });
	ImGui::SameLine();
	if (ImGui::BeginCombo(listLabel, previewImg ? content.getTexture(selectedTexID).getName().c_str() : nullptr))
	{
		if (ImGui::Selectable("Reset"))
		{
			(instance.*pFunction)(Okay::INVALID_UINT);
			pressed = true;
		}
		else
			content.forEachTexture(lambdaSelectTex);

		ImGui::EndCombo();
	}

	return pressed;
}