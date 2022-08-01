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
	void Save();

	bool TryImport(const std::string_view& path);

#pragma region 
public: // Getters
	std::shared_ptr<Okay::Mesh> GetMesh(const std::string& fileName);
	UINT GetNumMeshes() const { return (UINT)meshes.size(); }
	const Okay::String& GetMeshName(UINT index);

	template<typename T>
	void ForEachMesh(T& function)
	{
		for (auto& mesh : meshes)
			function(*mesh.second.get());
	}

private:
	bool AddMesh(const std::string& fileName);
	bool MeshExists(const std::string& fileName);

#pragma endregion Meshes


#pragma region
public: // Getters
	std::shared_ptr<Okay::Texture> GetTexture(const std::string& fileName);
	UINT GetNumTextures() const { return (UINT)textures.size(); }
	const Okay::String& GetTextureName(UINT index);
	
	template<typename T>
	void ForEachTexture(T& function)
	{
		for (auto& texture : textures)
			function(*texture.second.get());
	}

private:
	bool AddTexture(const std::string& fileName);
	bool TextureExists(const std::string& fileName);
#pragma endregion Textures


#pragma region Materials
public: // Getters
	std::shared_ptr<Okay::Material> GetMaterial(const std::string& materialName);
	std::shared_ptr<Okay::Material> GetMaterial(UINT index);
	const Okay::String& GetMaterialName(UINT index);
	UINT GetNumMaterials() const { return (UINT)materials.size(); }

	void ChangeMaterialName(const Okay::String& material, const Okay::String& name);
	void RemoveMaterial(const std::string& material);

	template<typename T>
	void ForEachMaterial(T& lambda)
	{
		for (auto& material : materials)
			lambda(*material.second.get());
	}

private:
	bool AddMaterial(const Okay::MaterialDesc_Strs& matDesc);
	bool AddMaterial(const Okay::MaterialDesc_Ptrs& matDesc);
	bool MaterialExists(const std::string& matName);
#pragma endregion Materials



private: // All loaded "assets"
	std::unordered_map<std::string, std::shared_ptr<Okay::Mesh>> meshes;
	std::unordered_map<std::string, std::shared_ptr<Okay::Texture>> textures;
	std::unordered_map<std::string, std::shared_ptr<Okay::Material>> materials;

private: // Asset loading and writing
	const Okay::String DeclarationPath = "../Content/Meshes/AssetDeclaration.okayDec";
	const std::string TexturePath = "../Content/Textures/";

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