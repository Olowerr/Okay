#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX

#include <unordered_map>
#include "Mesh.h"
#include "OkayImporter.h"

// Owns all Assets (Meshes, Textures, etc)
class Assets
{
private:
	Assets();
public:
	~Assets();
	Assets(const Assets&) = delete;
	Assets(Assets&&) = delete;
	Assets& operator=(const Assets&) = delete;

	static Assets& Get()
	{
		static Assets assets;
		return assets;
	}
	
	static bool AddMesh(const std::string& fileName) { return Get().AddMeshInternal(fileName); }
	static std::shared_ptr<Okay::Mesh> GetMesh(const std::string& fileName) { return Get().GetMeshInternal(fileName); }


private: // Internal Functions
	bool AddMeshInternal(const std::string& fileName);
	std::shared_ptr<Okay::Mesh> GetMeshInternal(const std::string& fileName);


private: // All loaded meshes
	std::unordered_map<std::string, std::shared_ptr<Okay::Mesh>> meshes;


private: // File loading and writing
	const Okay::String DeclarationPath = "../Assets/Meshes/AssetDeclaration.okayDec";
	bool LoadAll();
	bool ReadDeclaration();
	bool WriteDeclaration();
	std::vector<Okay::String> files;


};