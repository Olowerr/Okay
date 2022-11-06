#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include "OkayImporter.h"
#include "Engine/Okay/OkayString.h"

//#define TEXTURE

class DX11;

namespace Okay
{
	class Content
	{
	public:
		Content();
		~Content();
		void setDx11(DX11* pDx11);

		bool tryImport(std::string_view path);

	private:
		DX11* pDx11;

		std::vector<std::shared_ptr<Mesh>> meshes;

		bool loadMesh(std::string_view path);
#ifdef TEXTURE
		bool loadTexture(std::string_view path);
#endif
	};
}