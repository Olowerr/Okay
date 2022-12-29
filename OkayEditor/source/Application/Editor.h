#pragma once
#include <Engine/Application/Application.h>
#include <Engine/Application/Scene.h>
#include <Engine/DirectX/DX11.h>
#include <Engine/Application/Entity.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

class Editor : public Application
{
public:
	Editor(std::string_view startScene);
	~Editor();

	// Inherited via Application
	virtual void run() override;

private:
	Okay::Scene scene;

	void update();
};