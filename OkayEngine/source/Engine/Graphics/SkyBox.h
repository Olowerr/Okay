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
		~SkyBox();
		void shutdown();

		bool create(std::string_view path);
		inline ID3D11ShaderResourceView* const* getTextureCubeSRV() const;

		inline const std::string& getTextureName() const;

	private:
		ID3D11Texture2D* pTextureCube;
		ID3D11ShaderResourceView* pTextureCubeSRV;

		std::string textureName;

		void copyImgSection(uint32_t* pTarget, uint32_t* pSource, uint32_t imgWidth, uint32_t readWidth, uint32_t readHeight);
	};

	inline ID3D11ShaderResourceView* const* SkyBox::getTextureCubeSRV() const { return &pTextureCubeSRV; }
	inline const std::string& SkyBox::getTextureName() const { return textureName; }
}