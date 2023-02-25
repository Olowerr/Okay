#include "ContentBrowser.h"

#define FIND_ASSET_PTR(vector, name, pAsset)\
for (size_t i = 0; i < vector.size(); i++)\
{\
	if (vector[i].getName() == name)\
		pAsset = &vector[i];\
}\


namespace Okay
{
	Mesh& ContentBrowser::getMesh(std::string_view assetName)
	{
		Mesh* pAsset = nullptr;
		FIND_ASSET_PTR(meshes, assetName, pAsset);
		OKAY_ASSERT(pAsset, "Could not find mesh");
		return *pAsset;
	}

	const Mesh& ContentBrowser::getMesh(std::string_view assetName) const
	{
		const Mesh* pAsset = nullptr;
		FIND_ASSET_PTR(meshes, assetName, pAsset);
		OKAY_ASSERT(pAsset, "Could not find mesh");
		return *pAsset;
	}

	uint32_t Okay::ContentBrowser::getMeshID(std::string_view meshName) const
	{
		for (size_t i = 0; i < meshes.size(); i++)
		{
			if (meshes[i].getName() == meshName)
				return (uint32_t)i;
		}

		return INVALID_UINT;
	}

	Texture& ContentBrowser::getTexture(std::string_view assetName)
	{
		Texture* pAsset = nullptr;
		FIND_ASSET_PTR(textures, assetName, pAsset);
		OKAY_ASSERT(pAsset, "Could not find texture");
		return *pAsset;
	}

	const Texture& ContentBrowser::getTexture(std::string_view assetName) const
	{
		const Texture* pAsset = nullptr;
		FIND_ASSET_PTR(textures, assetName, pAsset);
		OKAY_ASSERT(pAsset, "Could not find texture");
		return *pAsset;
	}

	Material& ContentBrowser::getMaterial(std::string_view assetName)
	{
		Material* pAsset = nullptr;
		FIND_ASSET_PTR(materials, assetName, pAsset);
		OKAY_ASSERT(pAsset, "Could not find material");
		return *pAsset;
	}

	const Material& ContentBrowser::getMaterial(std::string_view assetName) const
	{
		const Material* pAsset = nullptr;
		FIND_ASSET_PTR(materials, assetName, pAsset);
		OKAY_ASSERT(pAsset, "Could not find material");
		return *pAsset;
	}

}