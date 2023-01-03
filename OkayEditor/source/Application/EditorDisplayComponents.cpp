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

#define IMGUI_DISPLAY_COMP_END()\
	ImGui::Separator();\
} 0 //maybe change lol

void Editor::displayComponents(Okay::Entity entity)
{
	using namespace Okay;

	IMGUI_DISPLAY_COMP_START(Transform, "Transform");

	// Display component
	Transform& tra = entity.getComponent<Transform>();
	
	// Make macro?
	ImGui::Text("Position:"); ImGui::SameLine();
	ImGui::DragFloat3("##TraposNL", &tra.position.x, 0.01f);
	
	ImGui::Text("Rotation:"); ImGui::SameLine();
	ImGui::DragFloat3("##TrarotNL", &tra.rotation.x, 0.01f);
	
	ImGui::Text("Scale:   "); ImGui::SameLine();
	ImGui::DragFloat3("##TrascaNL", &tra.scale.x, 0.01f);

	IMGUI_DISPLAY_COMP_END();



	IMGUI_DISPLAY_COMP_START(MeshComponent, "Mesh Display");
	
	MeshComponent& meshC = entity.getComponent<MeshComponent>();
	
	ImGui::Text("Mesh:	   %s", content.getMesh(meshC.meshIdx).getName().c_str());
	ImGui::Text("Material: %s", content.getMaterial(meshC.materialIdx).getName().c_str());
	
	IMGUI_DISPLAY_COMP_END();



	IMGUI_DISPLAY_COMP_START(Camera, "Camera");

	Camera& camera = entity.getComponent<Camera>();
	ImGui::Text("Coming soon TM...");

	IMGUI_DISPLAY_COMP_END();



	IMGUI_DISPLAY_COMP_START(PointLight, "Point Light");
	
	PointLight& pointLight = entity.getComponent<PointLight>();

	ImGui::Text("Intensity:"); ImGui::SameLine();
	ImGui::DragFloat("##PLintNL", &pointLight.intensity, 0.01f, 0.f, 100.f, nullptr, ImGuiSliderFlags_Logarithmic);

	ImGui::Text("Attenuation:"); ImGui::SameLine();
	ImGui::DragFloat2("##PLattNL", &pointLight.attenuation.x, 0.001f, 0.f, 1.f, nullptr, ImGuiSliderFlags_Logarithmic);

	ImGui::Text("Colour:"); //ImGui::SameLine();
	ImGui::ColorEdit3("##PLcolNL", &pointLight.colour.x);

	IMGUI_DISPLAY_COMP_END();
}