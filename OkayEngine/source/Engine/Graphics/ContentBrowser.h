#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include "Engine/Okay/Okay.h"
#include "Assets/Importing/OkayImporter.h"
#include "Assets/Texture.h"

namespace Okay
{
	class ContentBrowser
	{
	public:
		ContentBrowser();
		~ContentBrowser();

		bool importFile(std::string_view path);

		inline Mesh& getMesh(uint32_t index);

		bool loadTexture(std::string_view path);

		static bool canLoadTexture(const char* path);
	private:

		std::vector<Mesh> meshes;
		std::vector<Texture> textures;

		bool loadMesh(std::string_view path);
	};

	inline Mesh& ContentBrowser::getMesh(uint32_t index)
	{
		OKAY_ASSERT(index < (uint32_t)meshes.size(), "Invalid index");
		return meshes[index];
	}
}