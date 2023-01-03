#include "Editor.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/MeshComponent.h"

#include "imgui/imgui.h"

#define IMGUI_DISPLAY_COMP_START(comp, displayText)\
if (entity.hasComponent<Okay::comp>())\
{\
	ImGui::Text(displayText);\
	0

#define IMGUI_DISPLAY_COMP_END() } 0 //maybe change lol

void Editor::displayComponents(Okay::Entity entity)
{
	IMGUI_DISPLAY_COMP_START(Transform, "Transform");
	// Display component
	Okay::Transform& tra = entity.getComponent<Okay::Transform>();
	
	ImGui::Text("Position:"); ImGui::SameLine();
	ImGui::DragFloat3("##posNL", &tra.position.x, 0.01f);
	
	ImGui::Text("Rotation:"); ImGui::SameLine();
	ImGui::DragFloat3("##rotNL", &tra.rotation.x, 0.01f);
	
	ImGui::Text("Scale:   "); ImGui::SameLine();
	ImGui::DragFloat3("##scaNL", &tra.scale.x, 0.01f);

	IMGUI_DISPLAY_COMP_END();



	IMGUI_DISPLAY_COMP_START(MeshComponent, "Mesh");
	IMGUI_DISPLAY_COMP_END();



	IMGUI_DISPLAY_COMP_START(Camera, "Camera");
	IMGUI_DISPLAY_COMP_END();



	IMGUI_DISPLAY_COMP_START(PointLight, "Point Light");
	IMGUI_DISPLAY_COMP_END();
}