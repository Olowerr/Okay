#pragma once
#include <Engine/Application/Application.h>
#include <Engine/Application/Scene.h>
#include <Engine/DirectX/DX11.h>
#include <Engine/Application/Entity.h>

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
	void endFrame();
	void update();
};