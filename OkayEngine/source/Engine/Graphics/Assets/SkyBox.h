#pragma once

#include "Engine/Okay/Okay.h"
#include "Engine/DirectX/DX11.h"

#include <memory>

namespace Okay
{
	class SkyBox
	{
	public:
		SkyBox();
		SkyBox(SkyBox&& other) noexcept;
		~SkyBox();
		void shutdown();

		inline bool setPositiveX(std::string_view path);
		inline bool setNegativeX(std::string_view path);
		inline bool setPositiveY(std::string_view path);
		inline bool setNegativeY(std::string_view path);
		inline bool setPositiveZ(std::string_view path);
		inline bool setNegativeZ(std::string_view path);

		bool create();

	private:
		bool verifyAndSetPath(uint32_t idx, std::string_view path);

		// Both std::vector and a raw pointer doesn't feel fitting here
		std::unique_ptr<std::string[]> texturePaths;

		ID3D11Texture2D* pTextureCube;
		ID3D11ShaderResourceView* pTextureCubeSRV;
	};

	bool SkyBox::setPositiveX(std::string_view path) { return verifyAndSetPath(0u, path); }
	bool SkyBox::setNegativeX(std::string_view path) { return verifyAndSetPath(1u, path); }
	bool SkyBox::setPositiveY(std::string_view path) { return verifyAndSetPath(2u, path); }
	bool SkyBox::setNegativeY(std::string_view path) { return verifyAndSetPath(3u, path); }
	bool SkyBox::setPositiveZ(std::string_view path) { return verifyAndSetPath(4u, path); }
	bool SkyBox::setNegativeZ(std::string_view path) { return verifyAndSetPath(5u, path); }
}