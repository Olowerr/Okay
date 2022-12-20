#include "ContentBrowser.h"

namespace Okay
{
	ContentBrowser::ContentBrowser()
	{
	}

	ContentBrowser::~ContentBrowser()
	{
	}

	bool ContentBrowser::importFile(std::string_view path)
	{
		const std::string_view fileEnding = path.substr(path.find_last_of('.'));
		bool result = false;

#ifdef TEXTURE
		if (Okay::Texture::IsValid(path))
			return loadTexture(path.data());
#endif

		if (fileEnding == ".fbx" || fileEnding == ".FBX" || fileEnding == ".obj" || fileEnding == ".OBJ")
			return loadMesh(path);

		return false;
	}

	bool ContentBrowser::loadMesh(std::string_view path)
	{
		std::string materialName;
		std::string textures[3];

		// Overrite old file / import new file
		Okay::Mesh::MeshInfo meshInfo;
		OKAY_VERIFY(Importer::Load(path, meshInfo, textures, materialName));
	
#ifdef TEXTURES
		if (textures[0].size())
			loadTexture(textures[0]);

		if (textures[1].size())
			loadTexture(textures[1]);

		if (textures[2].size())
			loadTexture(textures[2]);
#endif

		meshes.emplace_back(meshInfo);
		
		return true;

		/*
		int x, y, c;
		unsigned char* pData = stbi_load(path.c_str(), &x, &y, &c, 4);

		if (!pData)
			return false;

		width = (UINT)x;
		height = (UINT)y;
		*ppData = pData;

		return true;
		*/

	}
}