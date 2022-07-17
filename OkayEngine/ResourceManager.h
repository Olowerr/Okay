#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX

#include <unordered_map>
#include "Mesh.h"
#include "OkayImporter.h"
#include "Texture.h"

// Owns all Assets (Meshes, Textures, etc)
class Assets
{
public:
	Assets();
	~Assets();
	Assets(const Assets&) = delete;
	Assets(Assets&&) = delete;
	Assets& operator=(const Assets&) = delete;

	bool AddMesh(const std::string& fileName);
	std::shared_ptr<Okay::Mesh> GetMesh(const std::string& fileName);

private: // All loaded meshes
	std::unordered_map<std::string, std::shared_ptr<Okay::Mesh>> meshes;


private: // File loading and writing
	const Okay::String DeclarationPath = "../Content/Meshes/AssetDeclaration.okayDec";
	bool LoadAll();
	bool ReadDeclaration();
	bool WriteDeclaration();
	std::vector<Okay::String> files;


};