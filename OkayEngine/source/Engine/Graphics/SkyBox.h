#pragma once

#include "Engine/Okay/Okay.h"
#include "Engine/DirectX/DX11.h"

#include <memory>

namespace Okay
{
	class SkyBox
	{
	public:
		static void init(ID3D11InputLayout* pPositionIL);

		SkyBox();
		SkyBox(SkyBox&& other) noexcept;
		~SkyBox();
		void shutdown();

		bool create(std::string_view path);

	private:
		ID3D11Texture2D* pTextureCube;
		ID3D11ShaderResourceView* pTextureCubeSRV;

		std::string texturePath;

		void copyImgSection(void* pTarget, uint32_t* pSource, uint32_t imgWidth, uint32_t readWidth, uint32_t readHeight);
			
	private: // statics used during rendering
		struct RenderResources
		{
			RenderResources() = default;
			~RenderResources() 
			{
				DX11_RELEASE(pPositionIL);
				DX11_RELEASE(pVS);
				DX11_RELEASE(pPS);
			}

			uint32_t cubeMeshID = INVALID_UINT;
			ID3D11InputLayout* pPositionIL = nullptr;
			ID3D11VertexShader* pVS = nullptr;
			ID3D11PixelShader* pPS = nullptr;

		};

		static std::unique_ptr<RenderResources> renderResources;

	};

}