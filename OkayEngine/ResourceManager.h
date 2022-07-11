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
	
	bool AddMesh(const std::string& fileName);
	std::shared_ptr<Okay::Mesh> GetMesh(const std::string& fileName);

	// Make Private
	bool LoadAll();
	bool ReadDeclaration();
	bool WriteDeclaration();
	std::vector<Okay::String> files;



private:
	std::unordered_map<std::string, std::shared_ptr<Okay::Mesh>> meshes;

	const Okay::String DeclarationPath = "../Assets/Meshes/AssetDeclaration.okayDec";


};