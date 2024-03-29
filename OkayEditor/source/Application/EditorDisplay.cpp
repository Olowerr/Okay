#include "Editor.h"
#include "EditorHelp.h"

#include <Engine/Components/Camera.h>
#include <Engine/Components/Transform.h>
#include <Engine/Components/MeshComponent.h>
#include <Engine/Components/SkyLight.h>

#include <imgui/imgui.h>

#include <filesystem>

void Editor::addComponents(Okay::Entity entity)
{
	using namespace Okay;

	ImGui::Text("");
	if (ImGui::BeginCombo("##NLAdd", "Add Component"))
	{
		IMGUI_ADD_COMP(Transform);
		IMGUI_ADD_COMP(MeshComponent);
		IMGUI_ADD_COMP(Camera);
		IMGUI_ADD_COMP(PointLight);
		IMGUI_ADD_COMP(DirectionalLight);
		IMGUI_ADD_COMP(SkyLight);

		ImGui::EndCombo();
	}
}

void Editor::displayComponents(Okay::Entity entity)
{
	using namespace Okay;

	ImGui::Text("Entity - %u", entity.getID());
	ImGui::Separator();

	// TODO: Change IMGUI_DISPLAY_COMP to a function, and make the components collapsable
	/*static const ImVec4 col1{0.3f, 0.3f, 0.3f, 1.f};
	static const ImVec4 col2{0.4f, 0.4f, 0.4f, 1.f};
	static const ImVec4 col3{0.2f, 0.2f, 0.2f, 1.f};

	ImGui::PushStyleColor(ImGuiCol_Header, col1);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, col2);
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, col3);

	if (ImGui::TreeNodeEx("la", ImGuiTreeNodeFlags_Framed, "asdasdad"))
		ImGui::TreePop();	

	ImGui::PopStyleColor(3);*/

	IMGUI_DISPLAY_COMP_START(Transform, "Transform", false);

	Transform& tra = entity.getComponent<Transform>();
	glm::vec3 degRot = glm::degrees(tra.rotation);

	// Make function / macro ?
	ImGui::Text("Position:"); 
	ImGui::SameLine();
	ImGui::DragFloat3("##NLTrapos", &tra.position.x, 0.01f, 0.f, 0.f, "%.1f");
	
	ImGui::Text("Rotation:"); 
	ImGui::SameLine();
	ImGui::DragFloat3("##NLTrarot", &degRot.x, 0.5f, 0.f, 0.f, "%.1f");
	
	ImGui::Text("Scale:   "); 
	ImGui::SameLine();
	ImGui::DragFloat3("##NLTrasca", &tra.scale.x, 0.01f, 0.f, 0.f, "%.1f");

	tra.rotation = glm::radians(degRot);

	IMGUI_DISPLAY_COMP_END();




	IMGUI_DISPLAY_COMP_START(MeshComponent, "Mesh", true);

	MeshComponent& meshC = entity.getComponent<MeshComponent>();
	
	ImGui::Text("Mesh:");
	selectAsset(meshC.meshIdx, meshC.meshIdx, content.getAll<Mesh>(), "##NLMesh");

	ImGui::Text("Material:");
	selectAsset(meshC.materialIdx, meshC.materialIdx, content.getAll<Material>(), "##NLMat");
	
	ImGui::Text("Shader:");
	selectAsset(meshC.shaderIdx, meshC.shaderIdx, content.getAll<Shader>(), "##NLSha");


	IMGUI_DISPLAY_COMP_END();



	IMGUI_DISPLAY_COMP_START(Camera, "Camera", true);

	Camera& camera = entity.getComponent<Camera>();
	
	float fovDeg = glm::degrees(camera.fov);
	ImGui::Text("FOV:");
	ImGui::SameLine();
	ImGui::DragFloat("##NLCFOV", &fovDeg, 0.02f, 0.f, 360.f);
	camera.fov = glm::radians(fovDeg);

	ImGui::Text("Near plane:");
	ImGui::SameLine();
	ImGui::DragFloat("##NLCNZ", &camera.nearZ, 0.02f, 0.f, 10.f);

	ImGui::Text("Far plane:");
	ImGui::SameLine();
	ImGui::DragFloat("##NLCFZ", &camera.farZ, 0.02f, 0.f, 5000.f);


	IMGUI_DISPLAY_COMP_END();


	IMGUI_DISPLAY_COMP_START(PointLight, "Point Light", true);
	
	PointLight& pointLight = entity.getComponent<PointLight>();

	ImGui::Text("Intensity:");
	ImGui::SameLine();
	ImGui::DragFloat("##NLPLint", &pointLight.intensity, 0.01f, 0.f, 100.f, nullptr, ImGuiSliderFlags_Logarithmic);

	ImGui::Text("Attenuation:");
	ImGui::SameLine();
	ImGui::DragFloat2("##NLPLatt", &pointLight.attenuation.x, 0.001f, 0.f, 1.f, nullptr, ImGuiSliderFlags_Logarithmic);

	ImGui::Text("Colour:"); 
	ImGui::ColorEdit3("##NLPLcol", &pointLight.colour.x);

	IMGUI_DISPLAY_COMP_END();


	IMGUI_DISPLAY_COMP_START(DirectionalLight, "Directional Light", true);
	
	DirectionalLight& dirLight = entity.getComponent<DirectionalLight>();

	ImGui::Text("Intensity:"); 
	ImGui::SameLine();
	ImGui::DragFloat("##NLDLInt", &dirLight.intensity, 0.01f, 0.f, 100.f, nullptr, ImGuiSliderFlags_Logarithmic);

	ImGui::Text("Colour:"); 
	ImGui::ColorEdit3("##NLDLcol", &dirLight.colour.x);

	IMGUI_DISPLAY_COMP_END();


	IMGUI_DISPLAY_COMP_START(SkyLight, "Sky Light", true);

	SkyLight& skyLight = entity.getComponent<SkyLight>();


	ImGui::Text("Ambient tint:"); 
	ImGui::ColorEdit3("##NLSKLtint", &skyLight.ambientTint.x);

	ImGui::Text("Ambient intensity:"); 
	ImGui::SameLine();
	ImGui::DragFloat("##NLSKLint", &skyLight.ambientIntensity, 0.01f, 0.f, 100.f, nullptr, ImGuiSliderFlags_Logarithmic);

	ImGui::Text("Sun colour:");
	ImGui::ColorEdit3("##NLSKLcol", &skyLight.sunColour.x);

	ImGui::Text("Size:");
	ImGui::SameLine();

	float displaySize = 100.f - skyLight.sunSize;
	ImGui::DragFloat("##NLSunSize", &displaySize, 0.1f, 0.1f, 99.9999f);
	skyLight.sunSize = 100.f - displaySize;

	ImGui::Text("Intensity:");
	ImGui::SameLine();
	ImGui::DragFloat("##NLSunint", &skyLight.sunIntensity, 0.1f);

	ImGui::Text("Skybox texture: %s", skyLight.skyBox->getTextureName().c_str());
	if (ImGui::Button("Select texture"))
	{
		std::string file;
		if (window.fileExplorerSelectFile(file))
		{
			if (!skyLight.skyBox->create(file))
				printf("Failed loading skybox texture\n"); // TODO: Add editor runtime warning & error
		}
	}

	IMGUI_DISPLAY_COMP_END();
}

void Editor::displayAssetList()
{
	using namespace Okay;
	static const ImVec2 size(250.f, 0.f);

	IMGUI_DISPLAY_ASSET_START(content.getAll<Mesh>(), Mesh, "Meshes", false, true);
	IMGUI_DISPLAY_ASSET_END();


	IMGUI_DISPLAY_ASSET_START(content.getAll<Texture>(), Texture, "Textures", true, true);
	IMGUI_DISPLAY_ASSET_END();


	IMGUI_DISPLAY_ASSET_START(content.getAll<Material>(), Material, "Materials", true, true);
	IMGUI_DISPLAY_ASSET_END();

	IMGUI_DISPLAY_ASSET_START(content.getAll<Shader>(), Shader, "Shaders", true, true);
	if (create)
	{
		selectionID = (uint32_t)content.getAmount<Shader>();
		selectionType = SelectionType::Shader;
		content.addAsset<Shader>();
	}
	IMGUI_DISPLAY_ASSET_END();
}

void Editor::displayMesh(uint32_t index)
{
	const Okay::Mesh& mesh = content.getAsset<Okay::Mesh>(index);

	ImGui::Text("Mesh - %s", mesh.getName().c_str());
	ImGui::Separator();

	ImGui::Text("Vertex count: %u", mesh.getNumIndices());
	ImGui::Text("Triangle count: %u", mesh.getNumIndices() / 3u);

	//TODO: Add rendered image of mesh

	ImGui::Separator();
}

void Editor::displayTexture(uint32_t index)
{
	const Okay::Texture& texture = content.getAsset<Okay::Texture>(index);
	const uint32_t width = texture.getWidth(), height = texture.getHeight();
	const float aspectRatio = texture.getAspectRatio();

	ImVec2 imgSize{};
	imgSize.x = ImGui::GetWindowWidth() - 25.f;
	imgSize.y = imgSize.x / aspectRatio;


	ImGui::Text("Texture - %s", texture.getName().c_str());
	ImGui::Separator();

	ImGui::Text("Width: %u", width);
	ImGui::Text("Height: %u", height);
	ImGui::Text("Aspect ratio: %.3f", aspectRatio);

	ImGui::Image(texture.getSRV(), imgSize);

	ImGui::Separator();
}

void Editor::displayMaterial(uint32_t index)
{
	Okay::Material& mat = content.getAsset<Okay::Material>(index);
	const uint32_t base = mat.getBaseColour();
	const uint32_t spec = mat.getSpecular();

	ImGui::Text("Material - %s", mat.getName().c_str());
	ImGui::Separator();

	ImGui::Text("Base Colour:"); ImGui::SameLine();
	if (selectTexture(mat, base, &Okay::Material::setBaseColour, "##NLBase"))
	{
		if (mat.getBaseColour() == Okay::INVALID_UINT)
			mat.setBaseColour(0u);
	}
	
	ImGui::Text("Specular:   "); ImGui::SameLine();
	if (selectTexture(mat, spec, &Okay::Material::setSpecular, "##NLSpec"))
	{
		if (mat.getSpecular() == Okay::INVALID_UINT)
			mat.setSpecular(0u);
	}

	ImGui::Text("");

	ImGui::Text("UV Offset:"); ImGui::SameLine();
	ImGui::DragFloat2("##NoOffsetLabel", &mat.getGPUData().uvOffset.x, 0.002f);

	ImGui::Text("UV Tiling:"); ImGui::SameLine();
	ImGui::DragFloat2("##NoTileLabel", &mat.getGPUData().uvTiling.x, 0.002f);

	ImGui::Text("Shinyness:"); ImGui::SameLine();
	ImGui::DragFloat("##NLshi", &mat.getGPUData().shinynessExp, 0.05f);

	ImGui::Separator();
}

void Editor::displayShader(uint32_t index)
{
	Okay::Shader& shader = content.getAsset<Okay::Shader>(index);
	const Okay::Texture* heightMap = shader.getHeightMap();
	static float heightMapScalar = 1.f;

	ImGui::Text("Shader - %s", shader.getName().c_str());
	ImGui::Separator();

	ImGui::Text("Height Map"); ImGui::SameLine();
	selectTexture(shader, shader.getHeightMapID(), &Okay::Shader::setHeightMap, "##NLheight");

	ImGui::Text("Height map scale: "); ImGui::SameLine();
	if (ImGui::DragFloat("##NLhms", &heightMapScalar, 0.01f))
		shader.setHeightMapScalar(heightMapScalar);

	ImGui::Text("Pixel Shader: %s", shader.getPSName().c_str());

	//if (ImGui::Button("Select"))
	//{
	//	std::string file;
	//	if (window.fileExplorerSelectFile(file))
	//		shader.compilePixelShader(file);
	//}
	//ImGui::SameLine();

	if (ImGui::Button("Reload"))
		shader.reloadShader();
}

void Editor::displayStyling()
{
	return;

	if (!ImGui::Begin("Styling"))
	{
		ImGui::End();
		return;
	}
	static float colEditWidth = -325.f;
	static float buttonWidth = 175.f;
	static float buttonOffset = 20.f;
	ImGui::PushItemWidth(colEditWidth);

	/*ImGui::DragFloat("ColorEdit width", &colEditWidth);
	ImGui::DragFloat("Button Width", &buttonWidth);
	ImGui::DragFloat("Button Offset", &buttonOffset);
	ImGui::Separator();*/

	ImGuiStyle& style = ImGui::GetStyle();
	IMGUI_DISPLAY_STYLE_COLOUR(Text);
	IMGUI_DISPLAY_STYLE_COLOUR(TextDisabled);
	IMGUI_DISPLAY_STYLE_COLOUR(WindowBg);
	IMGUI_DISPLAY_STYLE_COLOUR(ChildBg);
	IMGUI_DISPLAY_STYLE_COLOUR(PopupBg);
	IMGUI_DISPLAY_STYLE_COLOUR(Border);
	IMGUI_DISPLAY_STYLE_COLOUR(BorderShadow);
	IMGUI_DISPLAY_STYLE_COLOUR(FrameBg);
	IMGUI_DISPLAY_STYLE_COLOUR(FrameBgHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(FrameBgActive);
	IMGUI_DISPLAY_STYLE_COLOUR(TitleBg);
	IMGUI_DISPLAY_STYLE_COLOUR(TitleBgActive);
	IMGUI_DISPLAY_STYLE_COLOUR(TitleBgCollapsed);
	IMGUI_DISPLAY_STYLE_COLOUR(MenuBarBg);
	IMGUI_DISPLAY_STYLE_COLOUR(ScrollbarBg);
	IMGUI_DISPLAY_STYLE_COLOUR(ScrollbarGrab);
	IMGUI_DISPLAY_STYLE_COLOUR(ScrollbarGrabHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(ScrollbarGrabActive);
	IMGUI_DISPLAY_STYLE_COLOUR(CheckMark);
	IMGUI_DISPLAY_STYLE_COLOUR(SliderGrab);
	IMGUI_DISPLAY_STYLE_COLOUR(SliderGrabActive);
	IMGUI_DISPLAY_STYLE_COLOUR(Button);
	IMGUI_DISPLAY_STYLE_COLOUR(ButtonHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(ButtonActive);
	IMGUI_DISPLAY_STYLE_COLOUR(Header);
	IMGUI_DISPLAY_STYLE_COLOUR(HeaderHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(HeaderActive);
	IMGUI_DISPLAY_STYLE_COLOUR(SeparatorHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(SeparatorActive);
	IMGUI_DISPLAY_STYLE_COLOUR(ResizeGrip);
	IMGUI_DISPLAY_STYLE_COLOUR(ResizeGripHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(ResizeGripActive);
	IMGUI_DISPLAY_STYLE_COLOUR(Tab);
	IMGUI_DISPLAY_STYLE_COLOUR(TabHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(TabActive);
	IMGUI_DISPLAY_STYLE_COLOUR(TabUnfocused);
	IMGUI_DISPLAY_STYLE_COLOUR(TabUnfocusedActive);
	IMGUI_DISPLAY_STYLE_COLOUR(DockingPreview);
	IMGUI_DISPLAY_STYLE_COLOUR(DockingEmptyBg);
	IMGUI_DISPLAY_STYLE_COLOUR(PlotLines);
	IMGUI_DISPLAY_STYLE_COLOUR(PlotLinesHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(PlotHistogram);
	IMGUI_DISPLAY_STYLE_COLOUR(PlotHistogramHovered);
	IMGUI_DISPLAY_STYLE_COLOUR(TextSelectedBg);
	IMGUI_DISPLAY_STYLE_COLOUR(DragDropTarget);
	IMGUI_DISPLAY_STYLE_COLOUR(NavHighlight);
	IMGUI_DISPLAY_STYLE_COLOUR(NavWindowingHighlight);
	IMGUI_DISPLAY_STYLE_COLOUR(NavWindowingDimBg);
	IMGUI_DISPLAY_STYLE_COLOUR(ModalWindowDimBg);

	ImGui::PopItemWidth();
	ImGui::End();
}