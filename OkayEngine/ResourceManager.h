#pragma once
#include <unordered_map>
#include "Mesh.h"

// Owns all Assets (Meshes, Textures, etc)
class Assets
{
public:
	static Assets& Get()
	{
		static Assets assets;
		return assets;
	}
	
private:
	std::unordered_map<UINT, Okay::Mesh> meshes;


private:
	Assets();
public:
	~Assets();
	Assets(const Assets&) = delete;
	Assets(Assets&&) = delete;
	Assets& operator=(const Assets&) = delete;
};