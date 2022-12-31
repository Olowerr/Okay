#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include "Engine/Okay/Okay.h"
#include "Assets/Importing/OkayImporter.h"
#include "Assets/Texture.h"
#include "Assets/Material.h"

namespace Okay
{
	class ContentBrowser
	{
	public:
		ContentBrowser();
		~ContentBrowser();

		bool importFile(std::string_view path);

		inline Mesh& getMesh(uint32_t index);
		inline const Mesh& getMesh(uint32_t index) const;
		
		inline Texture& getTexture(uint32_t index);
		inline const Texture& getTexture(uint32_t index) const;

		inline Material& getMaterial(uint32_t index);
		inline const Material& getMaterial(uint32_t index) const;

		static bool canLoadTexture(const char* path);
	private:

		std::vector<Mesh> meshes;
		std::vector<Texture> textures;
		std::vector<Material> materials;

		bool loadMesh(std::string_view path);
		bool loadTexture(std::string_view path);
	};

	inline Mesh& ContentBrowser::getMesh(uint32_t index)
	{
		OKAY_ASSERT(index < (uint32_t)meshes.size(), "Invalid index");
		return meshes[index];
	}

	inline const Mesh& ContentBrowser::getMesh(uint32_t index) const
	{
		OKAY_ASSERT(index < (uint32_t)meshes.size(), "Invalid index");
		return meshes[index];
	}

	inline Texture& ContentBrowser::getTexture(uint32_t index)
	{
		OKAY_ASSERT(index < (uint32_t)textures.size(), "Invalid index");
		return textures[index];
	}

	inline const Texture& ContentBrowser::getTexture(uint32_t index) const
	{
		OKAY_ASSERT(index < (uint32_t)textures.size(), "Invalid index");
		return textures[index];
	}
	
	inline Material& ContentBrowser::getMaterial(uint32_t index)
	{
		OKAY_ASSERT(index < (uint32_t)materials.size(), "Invalid index");
		return materials[index];
	}

	inline const Material& ContentBrowser::getMaterial(uint32_t index) const
	{
		OKAY_ASSERT(index < (uint32_t)materials.size(), "Invalid index");
		return materials[index];
	}

}