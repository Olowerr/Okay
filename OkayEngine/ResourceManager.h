#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX

#include <unordered_map>
#include "Mesh.h"
#include "OkayImporter.h"
#include "Material.h"

// Owns all Assets (Meshes, Textures, etc)
class Assets
{
public:
	Assets();
	~Assets();
	Assets(const Assets&) = delete;
	Assets(Assets&&) = delete;
	Assets& operator=(const Assets&) = delete;

	void SetUp();

	bool TryImport(const std::string_view& path);

#pragma region 

	bool AddMesh(const std::string& fileName);
	bool MeshExists(const std::string& fileName);
	
	// Getters
	std::shared_ptr<Okay::Mesh> GetMesh(const std::string& fileName);
	UINT GetNumMeshes() const { return (UINT)meshes.size(); }
	const Okay::String& GetMeshName(UINT index);

#pragma endregion Meshes


#pragma region
	// Atm relative to TempObjFbx
	bool AddTexture(const std::string& fileName);
	bool TextureExists(const std::string& fileName);

	// Getters
	std::shared_ptr<Okay::Texture> GetTexture(const std::string& fileName);
	UINT GetNumTextures() const { return (UINT)textures.size(); }
	const Okay::String& GetTextureName(UINT index);
#pragma endregion Textures


#pragma region Materials
	bool AddMaterial(const Okay::MaterialDesc_Strs& matDesc);
	bool AddMaterial(const Okay::MaterialDesc_Ptrs& matDesc);
	bool MaterialExists(const std::string& matName);

	// Getters
	std::shared_ptr<Okay::Material> GetMaterial(const std::string& materialName);
	UINT GetNumMaterials() const { return (UINT)materials.size(); }
	const Okay::String& GetMaterialName(UINT index);
#pragma endregion Materials



private: // All loaded "assets"
	std::unordered_map<std::string, std::shared_ptr<Okay::Mesh>> meshes;
	std::unordered_map<std::string, std::shared_ptr<Okay::Texture>> textures;
	std::unordered_map<std::string, std::shared_ptr<Okay::Material>> materials;

private: // Asset loading and writing
	const Okay::String DeclarationPath = "../Content/Meshes/AssetDeclaration.okayDec";
	bool LoadDeclared();
	void ClearDeclared();
	bool ReadDeclaration();
	bool WriteDeclaration();
	std::vector<Okay::String> decMeshes;
	std::vector<Okay::String> decTextures;
	std::vector<Okay::MaterialDesc_Strs> decMaterials;
};

/*

	Decleration:

	UINT : NumMeshes
		Okay::String : MeshName : x NumMeshes

	UINT : NumTextures
		Okay::String : TexturePath : x NumTextures

	UINT : NumMaterials
		Okay::MaterialDesc_Strs : MaterialDesc : x NumMaterials

*/