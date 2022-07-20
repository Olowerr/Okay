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

	bool AddMesh(const std::string& fileName);
	std::shared_ptr<Okay::Mesh> GetMesh(const std::string& fileName);

	bool AddTexture(const std::string& fileName);
	std::shared_ptr<Okay::Texture> GetTexture(const std::string& fileName);

	bool AddMaterial(const Okay::MaterialDesc_Strs& matDesc, const std::string& materialName);
	bool AddMaterial(const Okay::MaterialDesc_Ptrs& matDesc, const std::string& materialName);
	std::shared_ptr<Okay::Material> GetMaterial(const std::string& materialName);

private: // All loaded "assets"
	std::unordered_map<std::string, std::shared_ptr<Okay::Mesh>> meshes;
	std::unordered_map<std::string, std::shared_ptr<Okay::Texture>> textures;
	std::unordered_map<std::string, std::shared_ptr<Okay::Material>> materials;

private: // Mesh loading and writing
	const Okay::String DeclarationPath = "../Content/Meshes/AssetDeclaration.okayDec";
	bool LoadAllMeshes();
	bool ReadDeclaration();
	bool WriteDeclaration();
	std::vector<Okay::String> files;


};