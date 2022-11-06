#include "Content.h"

namespace Okay
{
	Content::Content()
		:pDx11(nullptr)
	{
	}

	Content::~Content()
	{
	}

	void Content::setDx11(DX11* pDx11)
	{
		this->pDx11 = pDx11;
	}

	bool Content::tryImport(std::string_view path)
	{
		const std::string_view fileEnding = path.substr(path.find_last_of('.'));
		bool result = false;

#ifdef TEXTURE
		if (Okay::Texture::IsValid(path))
			result = loadTexture(path.data());
#endif

		if (fileEnding == ".fbx" || fileEnding == ".FBX" || fileEnding == ".obj" || fileEnding == ".OBJ")
			result = loadMesh(path);

		OKAY_VERIFY(result);

		return true;
	}

	bool Content::loadMesh(std::string_view path)
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

		// Creating mesh first 
		meshes.emplace_back(std::make_shared<Mesh>(meshInfo, pDx11));
		
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