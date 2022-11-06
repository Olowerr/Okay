#pragma once
#include <string>

#include "Engine/DirectX/DX11.h"

namespace Okay
{
	class Texture
	{
	public:
		Texture(const unsigned char* pData, uint32 width, uint32 height, std::string_view name);

		~Texture();
		void shutdown();

		void setName(std::string_view name)	{ this->name = name; }
		const std::string& getName() const	{ return name; }

		uint32 getWidth() const				{ return width; }
		uint32 getHeight() const			{ return height; }
		float getAspectRatio() const		{ return float(width) / float(height); }

		ID3D11Texture2D* const* getTexture() const			{ return &texture; }
		ID3D11ShaderResourceView* const* getSRV() const		{ return &srv; }

	private:
		std::string name;
		uint32 width, height;

		ID3D11Texture2D* texture;
		ID3D11ShaderResourceView* srv;

		D3D11_TEXTURE2D_DESC createDefaultDesc() const
		{
			// Width, height, mipLevels, arraySize, format, sampleDesc, usage, bindFlags, cpuAccess, miscFlags
			return { width, height, 1,1, DXGI_FORMAT_R8G8B8A8_UNORM, {1, 0}, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0 };
		}
	};
}