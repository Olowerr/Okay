#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include "OkayImporter.h"

//#define TEXTURE

class DX11;

namespace Okay
{
	class Content
	{
	public:
		Content();
		~Content();

		bool importFile(std::string_view path);

	private:
		std::vector<Mesh> meshes;

		bool loadMesh(std::string_view path);
#ifdef TEXTURE
		bool loadTexture(std::string_view path);
#endif
	};
}