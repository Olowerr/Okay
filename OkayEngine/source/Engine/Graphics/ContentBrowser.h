#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include "Assets/Importing/OkayImporter.h"


namespace Okay
{
	class ContentBrowser
	{
	public:
		ContentBrowser();
		~ContentBrowser();

		bool importFile(std::string_view path);

	private:
		std::vector<Mesh> meshes;

		bool loadMesh(std::string_view path);
#ifdef TEXTURE
		bool loadTexture(std::string_view path);
#endif
	};
}