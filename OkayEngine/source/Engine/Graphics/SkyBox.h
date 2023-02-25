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

	inline bool SkyBox::setPositiveX(std::string_view path) { return verifyAndSetPath(0u, path); }
	inline bool SkyBox::setNegativeX(std::string_view path) { return verifyAndSetPath(1u, path); }
	inline bool SkyBox::setPositiveY(std::string_view path) { return verifyAndSetPath(2u, path); }
	inline bool SkyBox::setNegativeY(std::string_view path) { return verifyAndSetPath(3u, path); }
	inline bool SkyBox::setPositiveZ(std::string_view path) { return verifyAndSetPath(4u, path); }
	inline bool SkyBox::setNegativeZ(std::string_view path) { return verifyAndSetPath(5u, path); }
}