#pragma once
#include "Engine/Okay/Okay.h"

#include <d3d11.h>

namespace Okay
{
	class RenderTexture
	{
	private:
		enum BitPos : uint32_t
		{
			B_BIT_8			 = 0,
			B_BIT_32		 = 1,
			B_RENDER		 = 0,
			B_SHADER_READ	 = 1,
			B_SHADER_WRITE	 = 2,
			B_DEPTH			 = 3,
		};
	public:

		enum Format : uint32_t
		{
			BIT_8	= 1 << BitPos::B_BIT_8,
			BIT_32	= 1 << BitPos::B_BIT_32,
		};

		enum Flags : uint32_t
		{
			RENDER			= 1 << BitPos::B_RENDER,
			SHADER_READ		= 1 << BitPos::B_SHADER_READ,
			SHADER_WRITE	= 1 << BitPos::B_SHADER_WRITE,
			DEPTH			= 1 << BitPos::B_DEPTH,
		};

		RenderTexture();
		RenderTexture(ID3D11Texture2D* texture, uint32_t flags);
		RenderTexture(uint32_t width, uint32_t height, uint32_t flags);
		~RenderTexture();
		void shutdown();

		void create(ID3D11Texture2D* texture, uint32_t flags);
		void create(uint32_t width, uint32_t height, uint32_t flags);

		void clear();
		void clear(float* colour);
		void clear(const glm::vec4& colour);

		glm::ivec2 getDimensions() const;

		inline ID3D11Texture2D* getBuffer();

		inline ID3D11RenderTargetView* getRTV();
		inline ID3D11RenderTargetView* const* getRTV() const;
		
		inline ID3D11ShaderResourceView* getSRV();
		inline ID3D11ShaderResourceView* const* getSRV() const;
		
		inline ID3D11UnorderedAccessView* getUAV();
		inline ID3D11UnorderedAccessView* const* getUAV() const;
		

		inline ID3D11Texture2D* getDepthBuffer();

		inline ID3D11DepthStencilView* getDSV();
		inline ID3D11DepthStencilView* const* getDSV() const;

	private:
		ID3D11Texture2D* buffer;
		ID3D11RenderTargetView* rtv;
		ID3D11ShaderResourceView* srv;
		ID3D11UnorderedAccessView* uav;

		ID3D11Texture2D* depthBuffer;
		ID3D11DepthStencilView* dsv;

		void readFlgs(uint32_t flags);
	};

	inline ID3D11Texture2D* RenderTexture::getBuffer()
		{ return buffer; }

	inline ID3D11RenderTargetView* RenderTexture::getRTV()
		{ return rtv; }

	inline ID3D11RenderTargetView* const* RenderTexture::getRTV() const
		{ return &rtv; }

	inline ID3D11ShaderResourceView* RenderTexture::getSRV()
		{ return srv; }

	inline ID3D11ShaderResourceView* const* RenderTexture::getSRV() const
		{ return &srv; }

	inline ID3D11UnorderedAccessView* RenderTexture::getUAV()
		{ return uav; }

	inline ID3D11UnorderedAccessView* const* RenderTexture::getUAV() const
		{ return &uav; }

	inline ID3D11Texture2D* RenderTexture::getDepthBuffer()
		{ return depthBuffer; }

	inline ID3D11DepthStencilView* RenderTexture::getDSV()
		{ return dsv; }

	inline ID3D11DepthStencilView* const* RenderTexture::getDSV() const
		{ return &dsv; }

} // Okay