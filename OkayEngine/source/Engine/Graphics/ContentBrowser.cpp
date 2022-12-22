#include "ContentBrowser.h"
#include "Assets/Importing/stb_image.h"

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
	
		if (textures[0].size())
			loadTexture(textures[0]);

		if (textures[1].size())
			loadTexture(textures[1]);

		if (textures[2].size())
			loadTexture(textures[2]);

		meshes.emplace_back(meshInfo);
		
		return true;

	}

	bool ContentBrowser::loadTexture(std::string_view path)
	{
		int x, y, c;
		unsigned char* pData = stbi_load(path.data(), &x, &y, &c, 4);

		if (!pData)
			return false;

		size_t pos = path.find_last_of('/');
		pos = pos == -1ull ? path.find_last_of('\\') : pos;
		
		// TODO: Verify texName is correct (probably not atm)
		const std::string_view texName = path.substr(pos);

		textures.emplace_back(pData, (uint32_t)x, (uint32_t)y, texName);

	}
}