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

		// TODO: Display warning if ANY importing fails

		if (ContentBrowser::canLoadTexture(path.data()))
			return loadTexture(path.data());

		// TODO: Find a more robust way of checking this. (Preferably ask Assimp if it can load the file)
		if (fileEnding == ".fbx" || fileEnding == ".FBX" || fileEnding == ".obj" || fileEnding == ".OBJ")
			return loadMesh(path);

		return false;
	}

	bool ContentBrowser::loadMesh(std::string_view path)
	{
		std::string materialName;
		std::string textures[3];

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
		OKAY_VERIFY(pData);

		size_t pos = path.find_last_of('/');
		pos = pos == std::string_view::npos ? path.find_last_of('\\') : pos;
		pos++;

		// Texture name contains ex: .png, it is removed inside the Texture constructor
		std::string_view texName = path.substr(pos);

		textures.emplace_back(pData, (uint32_t)x, (uint32_t)y, texName);

		return true;
	}
	
	bool ContentBrowser::canLoadTexture(const char* path)
	{
		int x, y, c;
		return stbi_info(path, &x, &y, &c);
	}
}