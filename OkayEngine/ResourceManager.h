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
public:
	static Assets& Get()
	{
		static Assets assets;
		return assets;
	}
	
	void AddMesh(const std::string& fileName);
	std::shared_ptr<Okay::Mesh> GetMesh(const std::string& fileName);

private:
	std::unordered_map<std::string, std::shared_ptr<Okay::Mesh>> meshes;




private:
	Assets();
public:
	~Assets();
	Assets(const Assets&) = delete;
	Assets(Assets&&) = delete;
	Assets& operator=(const Assets&) = delete;
};