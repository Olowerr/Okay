#include "Editor.h"
#include "EditorHelp.h"

#include <Engine/Components/Camera.h>
#include <Engine/Components/Transform.h>
#include <Engine/Components/MeshComponent.h>
#include <Engine/Okay/OkayString.h>

#include <imgui/imgui.h>

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

		ImGui::EndCombo();
	}
}

void Editor::displayComponents(Okay::Entity entity)
{
	using namespace Okay;

	IMGUI_DISPLAY_COMP_START(Transform, "Transform", false);

	Transform& tra = entity.getComponent<Transform>();

	// Make macro?
	ImGui::Text("Position:"); ImGui::SameLine();
	ImGui::DragFloat3("##TraposNL", &tra.position.x, 0.01f);
	
	ImGui::Text("Rotation:"); ImGui::SameLine();
	ImGui::DragFloat3("##TrarotNL", &tra.rotation.x, 0.01f);
	
	ImGui::Text("Scale:   "); ImGui::SameLine();
	ImGui::DragFloat3("##TrascaNL", &tra.scale.x, 0.01f);

	IMGUI_DISPLAY_COMP_END();




	IMGUI_DISPLAY_COMP_START(MeshComponent, "Mesh", true);

	MeshComponent& meshC = entity.getComponent<MeshComponent>();
	
	ImGui::Text("Mesh: %s", content.getMesh(meshC.meshIdx).getName().c_str());
	selectAsset(meshC.meshIdx, meshC.meshIdx, content.getMeshes(), "##NLMesh");

	ImGui::Text("Material: %s", content.getMaterial(meshC.materialIdx).getName().c_str());
	selectAsset(meshC.materialIdx, meshC.materialIdx, content.getMaterials(), "##NLMat");
	
	ImGui::Text("Shader: %s", content.getShader(meshC.shaderIdx).getName().c_str());
	selectAsset(meshC.shaderIdx, meshC.shaderIdx, content.getShaders(), "##NLSha");


	IMGUI_DISPLAY_COMP_END();



	IMGUI_DISPLAY_COMP_START(Camera, "Camera", true);

	Camera& camera = entity.getComponent<Camera>();
	ImGui::Text("Coming soon...");

	IMGUI_DISPLAY_COMP_END();


	IMGUI_DISPLAY_COMP_START(PointLight, "Point Light", true);
	
	PointLight& pointLight = entity.getComponent<PointLight>();

	ImGui::Text("Intensity:"); ImGui::SameLine();
	ImGui::DragFloat("##PLintNL", &pointLight.intensity, 0.01f, 0.f, 100.f, nullptr, ImGuiSliderFlags_Logarithmic);

	ImGui::Text("Attenuation:"); ImGui::SameLine();
	ImGui::DragFloat2("##PLattNL", &pointLight.attenuation.x, 0.001f, 0.f, 1.f, nullptr, ImGuiSliderFlags_Logarithmic);

	ImGui::Text("Colour:"); //ImGui::SameLine();
	ImGui::ColorEdit3("##PLcolNL", &pointLight.colour.x);

	IMGUI_DISPLAY_COMP_END();
}

void Editor::displayAssetList()
{
	using namespace Okay;
	static const ImVec2 size(250.f, 0.f);

	IMGUI_DISPLAY_ASSET_START(content.getMeshes(), Mesh, "Meshes", false, true);
	IMGUI_DISPLAY_ASSET_END();


	IMGUI_DISPLAY_ASSET_START(content.getTextures(), Texture, "Textures", true, true);
	IMGUI_DISPLAY_ASSET_END();


	IMGUI_DISPLAY_ASSET_START(content.getMaterials(), Material, "Materials", true, true);
	IMGUI_DISPLAY_ASSET_END();

	IMGUI_DISPLAY_ASSET_START(content.getShaders(), Shader, "Shaders", true, true);
	if (create)
	{
		selectionID = (uint32_t)content.getShaders().size();
		selectionType = SelectionType::Shader;
		content.addShader(content, "New shader");
	}
	IMGUI_DISPLAY_ASSET_END();
}

void Editor::displayMesh(uint32_t index)
{
	const Okay::Mesh& mesh = content.getMesh(index);

	ImGui::Text("Mesh - %s", mesh.getName().c_str());
	ImGui::Separator();

	ImGui::Text("Vertex count: %u", mesh.getNumIndices());
	ImGui::Text("Triangle count: %u", mesh.getNumIndices() / 3u);

	//TODO: Add rendered image of mesh

	ImGui::Separator();
}

void Editor::displayTexture(uint32_t index)
{
	const Okay::Texture& texture = content.getTexture(index);
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
	Okay::Material& mat = content.getMaterial(index);
	const uint32_t base = mat.getBaseColour();
	const uint32_t spec = mat.getSpecular();
	const uint32_t ambi = mat.getAmbient();

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

	ImGui::Text("Ambient:    "); ImGui::SameLine();
	if (selectTexture(mat, ambi, &Okay::Material::setAmbient, "##NLAmbi"))
	{
		if (mat.getAmbient() == Okay::INVALID_UINT)
			mat.setAmbient(0u);
	}

	ImGui::Text("");

	ImGui::Text("UV Offset:"); ImGui::SameLine();
	ImGui::DragFloat2("##NoOffsetLabel", &mat.getGPUData().uvOffset.x, 0.002f);

	ImGui::Text("UV Tiling:"); ImGui::SameLine();
	ImGui::DragFloat2("##NoTileLabel", &mat.getGPUData().uvTiling.x, 0.002f);

	ImGui::Separator();
}

void Editor::displayShader(uint32_t index)
{
	Okay::Shader& shader = content.getShader(index);
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

}