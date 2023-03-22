#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include "Engine/Okay/Okay.h"
#include "Assets/Importing/OkayImporter.h"
#include "Assets/Texture.h"
#include "Assets/Material.h"
#include "Assets/Shader.h"

// TODO: Move STBI to deps

namespace Okay
{
	class ContentBrowser // TODO: Clean up 
	{
	private:
		ContentBrowser();
	public:
		ContentBrowser(const ContentBrowser&) = delete;
		ContentBrowser(ContentBrowser&&) = delete;
		ContentBrowser& operator=(const ContentBrowser&) = delete;
		~ContentBrowser();

		static inline ContentBrowser& get()
		{
			static ContentBrowser cb;
			return cb;
		}

		static bool canLoadTexture(const char* path);

		bool importFile(std::string_view path);

		template<typename Func, typename... Args>
		void forEachMesh(const Func& function, Args&&... args);

		template<typename... Args>
		inline Mesh& addMesh(Args&&... args);

		inline uint32_t getNumMeshes() const;
		inline const std::vector<Mesh>& getMeshes() const;
		inline Mesh& getMesh(uint32_t index);
		inline const Mesh& getMesh(uint32_t index) const;
		uint32_t getMeshID(std::string_view meshName) const;
		Mesh& getMesh(std::string_view meshName);
		const Mesh& getMesh(std::string_view meshName) const;


		template<typename Func, typename... Args>
		void forEachTexture(const Func& function, Args&&... args);

		inline uint32_t getNumTextures() const;
		inline const std::vector<Texture>& getTextures() const;
		inline Texture& getTexture(uint32_t index);
		inline const Texture& getTexture(uint32_t index) const;
		Texture& getTexture(std::string_view textureName);
		const Texture& getTexture(std::string_view textureName) const;


		template<typename Func, typename... Args>
		void forEachMaterial(const Func& function, Args&&... args);

		template<typename... Args>
		inline Material& addMaterial(Args&&... args);

		inline uint32_t getNumMaterials() const;
		inline const std::vector<Material>& getMaterials() const;
		inline Material& getMaterial(uint32_t index);
		inline const Material& getMaterial(uint32_t index) const;
		Material& getMaterial(std::string_view materialName);
		const Material& getMaterial(std::string_view materialName) const;

		template<typename Func, typename... Args>
		void forEachShader(const Func& function, Args&&... args);

		template<typename... Args>
		Shader& addShader(Args&&... args);
		inline const std::vector<Shader>& getShaders() const;
		inline const Shader& getShader(uint32_t index) const;
		inline Shader& getShader(uint32_t index);
		inline uint32_t getNumShaders() const;

	private:
		std::vector<Mesh> meshes;
		std::vector<Texture> textures;
		std::vector<Material> materials;
		std::vector<Shader> shaders;

		bool loadMesh(std::string_view path);
		bool loadTexture(std::string_view path);
	};

	template<typename Func, typename... Args>
	inline void ContentBrowser::forEachMesh(const Func& function, Args&&... args)
	{
		for (size_t i = 0; i < meshes.size(); i++)
			function(meshes[i], args...);
	}

	template<typename ...Args>
	inline Mesh& ContentBrowser::addMesh(Args && ...args)
	{
		return meshes.emplace_back(args...);
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

	template<typename ...Args>
	inline Material& ContentBrowser::addMaterial(Args && ...args)
	{
		return materials.emplace_back(args...);
	}

	template<typename Func, typename ...Args>
	inline void ContentBrowser::forEachShader(const Func& function, Args && ...args)
	{
		for (size_t i = 0; i < shaders.size(); i++)
			function(shaders[i], args...);
	}

	template<typename ...Args>
	inline Shader& ContentBrowser::addShader(Args&&... args)
	{
		return shaders.emplace_back(args...);
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

	inline uint32_t ContentBrowser::getNumMeshes() const
	{
		return (uint32_t)meshes.size();
	}

	inline const std::vector<Mesh>& ContentBrowser::getMeshes() const
	{
		return meshes;
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

	inline uint32_t ContentBrowser::getNumTextures() const
	{
		return (uint32_t)textures.size();
	}

	inline const std::vector<Texture>& ContentBrowser::getTextures() const
	{
		return textures;
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

	inline uint32_t ContentBrowser::getNumMaterials() const
	{
		return (uint32_t)materials.size();
	}

	inline const std::vector<Material>& ContentBrowser::getMaterials() const
	{
		return materials;
	}

	inline const std::vector<Shader>& Okay::ContentBrowser::getShaders() const
	{
		return shaders;
	}

	inline const Shader& ContentBrowser::getShader(uint32_t index) const
	{
		OKAY_ASSERT(index < (uint32_t)shaders.size(), "Invalid index");
		return shaders[index];
	}

	inline Shader& ContentBrowser::getShader(uint32_t index)
	{
		OKAY_ASSERT(index < (uint32_t)shaders.size(), "Invalid index");
		return shaders[index];
	}

	inline uint32_t ContentBrowser::getNumShaders() const
	{
		return (uint32_t)shaders.size();
	}

}