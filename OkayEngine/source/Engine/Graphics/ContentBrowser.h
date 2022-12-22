#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include "Assets/Importing/OkayImporter.h"
#include "Engine/Okay/Okay.h"

namespace Okay
{
	class Texture;

	class ContentBrowser
	{
	public:
		ContentBrowser();
		~ContentBrowser();

		bool importFile(std::string_view path);

		inline Mesh& getMesh(uint32_t index);

	private:
		std::vector<Mesh> meshes;
		std::vector<Texture> textures;

		bool loadMesh(std::string_view path);
		bool loadTexture(std::string_view path);
	};

	inline Mesh& ContentBrowser::getMesh(uint32_t index)
	{
		OKAY_ASSERT(index < (uint32_t)meshes.size(), "Invalid index");
		return meshes[index];
	}
}