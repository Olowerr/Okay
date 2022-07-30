#pragma once
#include "DX11.h"

#include "stb_image.h"

namespace Okay
{
	class Texture
	{
	public:
		Texture(); // Maybe default constructor shouldn't do anything..
		Texture(UINT width, UINT height, DXGI_FORMAT format, UINT bindFlags, const String& name = "Default");
		Texture(const std::string& path, UINT bindFlags = D3D11_BIND_SHADER_RESOURCE);

		~Texture();
		void Shutdown();

		void SetName(const String& name)			{ this->name = name; }
		const String& GetName()						{ return name; }
		bool GetIsValid() const						{ return isValid; }
		UINT GetWidth() const						{ return width; }
		UINT GetHeight() const						{ return height; }

		ID3D11Texture2D* const* GetTexture()		{ return &texture; }
		ID3D11ShaderResourceView* const* GetSRV()	{ return &srv; }
		ID3D11RenderTargetView* const* GetRTV()		{ return &rtv; }
		ID3D11UnorderedAccessView* const* GetUAV()	{ return &uav; }

		// Helper
		static bool IsValid(const std::string& texturePath)
		{
			return stbi_info(texturePath.c_str(), nullptr, nullptr, nullptr);
		}

	private:
		String name;
		UINT width, height;
		bool isValid;

		ID3D11Texture2D* texture;
		ID3D11ShaderResourceView* srv;
		ID3D11RenderTargetView* rtv;
		ID3D11UnorderedAccessView* uav;

		D3D11_TEXTURE2D_DESC CreateDefaultDesc()
		{
			// Width, height, mipLevels, arraySize, format, sampleDesc, usage, bindFlags, cpuAccess, miscFlags
			return { width, height, 1,1,DXGI_FORMAT_R8G8B8A8_UNORM, {1, 0}, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0 };
		}
		bool LoadTexture(const std::string& path, unsigned char** ppData);
		void CreateViews(UINT bindFlags);
	};
}