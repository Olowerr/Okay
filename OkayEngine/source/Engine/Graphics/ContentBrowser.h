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

		template<typename Func, typename... Args>
		void forEachMesh(const Func& function, Args&&... args);

		inline Mesh& getMesh(uint32_t index);
		inline const Mesh& getMesh(uint32_t index) const;
		inline size_t getNumMeshes() const;


		template<typename Func, typename... Args>
		void forEachTexture(const Func& function, Args&&... args);

		inline Texture& getTexture(uint32_t index);
		inline const Texture& getTexture(uint32_t index) const;
		inline size_t getNumTextures() const;


		template<typename Func, typename... Args>
		void forEachMaterial(const Func& function, Args&&... args);

		inline Material& getMaterial(uint32_t index);
		inline const Material& getMaterial(uint32_t index) const;
		inline size_t getNumMaterials() const;


		static bool canLoadTexture(const char* path);

	private:

		std::vector<Mesh> meshes;
		std::vector<Texture> textures;
		std::vector<Material> materials;

		bool loadMesh(std::string_view path);
		bool loadTexture(std::string_view path);
	};

	template<typename Func, typename... Args>
	inline void ContentBrowser::forEachMesh(const Func& function, Args&&... args)
	{
		for (size_t i = 0; i < meshes.size(); i++)
			function(meshes[i], args...);
	}
	
	template<typename Func, typename... Args>
	inline void ContentBrowser::forEachTexture(const Func& function, Args&&... args)
	{
		for (size_t i = 0; i < textures.size(); i++)
			function(textures[i], args...);
	}
	
	template<typename Func, typename... Args>
	inline void ContentBrowser::forEachMaterial(const Func& function, Args&&... args)
	{
		for (size_t i = 0; i < materials.size(); i++)
			function(materials[i], args...);
	}

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

	inline size_t ContentBrowser::getNumMeshes() const
	{
		return meshes.size();
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

	inline size_t ContentBrowser::getNumTextures() const
	{
		return textures.size();
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

	inline size_t ContentBrowser::getNumMaterials() const
	{
		return materials.size();
	}



}