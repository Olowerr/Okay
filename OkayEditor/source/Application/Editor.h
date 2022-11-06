#pragma once
#include <Engine/Application/Application.h>
#include <Engine/Application/Scene.h>
#include <Engine/DirectX/DX11.h>

class Editor : public Application
{
public:
	Editor(std::string_view scene);
	~Editor();

	// Inherited via Application
	virtual void run() override;

private:
	Scene scene;
};