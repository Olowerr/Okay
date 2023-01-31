#pragma once

// Components

#define IMGUI_ADD_COMP(comp)\
if (!entity.hasComponent<comp>())\
{\
	if (ImGui::Selectable(#comp, false))\
		entity.addComponent<Okay::comp>();\
}0

//bool rmvComp = ImGui::ImageButton(content.getTexture(XIconID).getSRV(), {ImVec2(15.f, 15.f)});
#define IMGUI_DISPLAY_COMP_START(comp, displayText, rmvButton)\
if (entity.hasComponent<Okay::comp>())\
{\
	ImGui::Text(displayText);\
	bool removed = false;\
	if (rmvButton)\
	{\
		ImGui::SameLine(ImGui::GetWindowWidth() - 52.f);\
		if (removed = ImGui::Button("RMV##" displayText))\
			entity.removeComponent<comp>();\
	}\
	if (!removed)\
	{0
	

#define IMGUI_DISPLAY_COMP_END()\
	ImGui::Separator(); }}0 


// Assets

#define IMGUI_DISPLAY_ASSET_START(vector, assetType, label, creB, rmvB)\
if (ImGui::BeginListBox("##NL" label, size))\
{\
	ImGui::Text(label ": ");\
	ImGui::SameLine();\
	bool create = false;\
	if (creB) { ImGui::SameLine(); create = ImGui::Button("Create##" label); }\
	bool remove = false;\
	if (rmvB) { ImGui::SameLine(); remove = ImGui::Button("Remove##" label); }\
	ImGui::Separator();\
	for (size_t i = 0; i < vector.size(); i++)\
	{\
		if (ImGui::Selectable(vector[i].getName().c_str(), (size_t)selectionID == i && selectionType == SelectionType::assetType))\
		{\
			selectionID = (uint32_t)i;\
			selectionType = SelectionType::assetType;\
		}\
	}

#define IMGUI_DISPLAY_ASSET_END() ImGui::EndListBox(); } ImGui::SameLine()


#define IMGUI_DISPLAY_STYLE_COLOUR(idx)\
ImGui::ColorEdit4(#idx, &style.Colors[ImGuiCol_##idx].x); ImGui::SameLine(ImGui::GetWindowWidth() - buttonWidth - buttonOffset);\
if (ImGui::Button("Print: "#idx, ImVec2(buttonWidth, 20.f))) printf("style.Colors[ImGuiCol_"#idx "] = ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);\n",\
style.Colors[ImGuiCol_##idx].x, style.Colors[ImGuiCol_##idx].y, style.Colors[ImGuiCol_##idx].z, style.Colors[ImGuiCol_##idx].w);