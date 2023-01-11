#pragma once
#include <Engine/Application/Application.h>
#include <Engine/Application/Scene.h>
#include <Engine/DirectX/DX11.h>
#include <Engine/Application/Entity.h>
#include <Engine/Graphics/Noise/PerlinNoise2D.h>

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

	enum struct SelectionType { None, Entity, Mesh, Texture, Material};
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

	void displayContent();
	void displayAssetList();
	void openFileExplorer();

	Okay::RenderTexture* testTex;
	Okay::PerlinNoise2D* noiser;
};