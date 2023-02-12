#include "SkyBox.h"
#include "Importing/stb_image.h"
#include "../ContentBrowser.h"

namespace Okay
{
	SkyBox::SkyBox()
		:pTextureCube(nullptr), pTextureCubeSRV(nullptr)
	{
		texturePaths = std::make_unique<std::string[]>(6ull);
	}

	SkyBox::SkyBox(SkyBox&& other) noexcept
		:texturePaths(std::move(other.texturePaths))
	{
		pTextureCube = other.pTextureCube;
		other.pTextureCube = nullptr;

		pTextureCubeSRV = other.pTextureCubeSRV;
		other.pTextureCubeSRV = nullptr;
	}

	SkyBox::~SkyBox()
	{
		shutdown();
	}

	void SkyBox::shutdown()
	{
		DX11_RELEASE(pTextureCube);
		DX11_RELEASE(pTextureCubeSRV);
		texturePaths.release();
	}

	bool SkyBox::create()
	{
		for (size_t i = 0; i < 6; i++)
		{
			if (texturePaths[i] == "")
				return false;
		}

		// TODO: Create texture cube

	}
	
	bool SkyBox::verifyAndSetPath(uint32_t idx, std::string_view path)
	{
		if (!ContentBrowser::canLoadTexture(path.data()))
			return false;

		texturePaths[idx] = path;
		return true;
	}
}
