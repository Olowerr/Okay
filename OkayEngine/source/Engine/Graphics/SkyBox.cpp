#include "SkyBox.h"

#include "ContentBrowser.h"
#include "Engine/Graphics/Assets/Importing/stb_image.h"

namespace Okay
{
	std::unique_ptr<SkyBox::RenderResources> SkyBox::renderResources;

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
	}

	bool SkyBox::create()
	{
		int width = 0, height = 0;

		// Use texture 0 as a reference for width and height
		if (!stbi_info(texturePaths[0].c_str(), &width, &height, nullptr))
			return false;

		int otherWidth = 0, otherHeight = 0;
		for (size_t i = 1; i < 6; i++)
		{
			if (!stbi_info(texturePaths[i].c_str(), &otherWidth, &otherHeight, nullptr))
				return false;

			if (width != otherWidth || height != otherHeight)
				return false;
		}

		// Reaching this means everything checks out
		D3D11_SUBRESOURCE_DATA data[6]{};

		for (size_t i = 0; i < 6; i++)
		{
			// We're still required to send in valid int-pointers for width and height, unless we wanna change STBI
			data[i].pSysMem = stbi_load(texturePaths[i].c_str(), &width, &height, nullptr, 4);
			data[i].SysMemPitch = uint32_t(width * 4);
			data[i].SysMemSlicePitch = 0u;
		}

		D3D11_TEXTURE2D_DESC desc{};
		desc.ArraySize = 6u;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0u;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;

		HRESULT hr = DX11::get().getDevice()->CreateTexture2D(&desc, data, &pTextureCube);
		for (size_t i = 0; i < 6; i++)
			stbi_image_free((void*)data[i].pSysMem);

		OKAY_ASSERT(SUCCEEDED(hr), "Failed creating Texture Cube");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = 1u;
		srvDesc.TextureCube.MostDetailedMip = 0u;

		hr = DX11::get().getDevice()->CreateShaderResourceView(pTextureCube, &srvDesc, &pTextureCubeSRV);
		if (FAILED(hr))
			pTextureCube->Release();

		OKAY_ASSERT(SUCCEEDED(hr), "Failed creating Texture Cube SRV");

		return true;
	}

	bool SkyBox::verifyAndSetPath(uint32_t idx, std::string_view path)
	{
		if (!ContentBrowser::canLoadTexture(path.data()))
			return false;

		texturePaths[idx] = path;
		return true;
	}

	void SkyBox::init(ID3D11InputLayout* pPositionIL)
	{
		OKAY_ASSERT(pPositionIL, "Position IL was nullptr");
		renderResources = std::make_unique<SkyBox::RenderResources>();

		ContentBrowser& content = ContentBrowser::get();

		renderResources->cubeMeshID = content.getMeshID("cube");
		if (renderResources->cubeMeshID == INVALID_UINT)
		{
			bool found = content.importFile(ENGINE_RESOURCES_PATH "cube.fbx");
			OKAY_ASSERT(found, "Failed loading cube.fbx");
			renderResources->cubeMeshID = content.getNumMeshes() - 1u;
		}

		pPositionIL->AddRef();
		renderResources->pPositionIL = pPositionIL;
		DX11::createVertexShader(SHADER_PATH "SkyBoxVS.cso", &renderResources->pVS);
		DX11::createPixelShader(SHADER_PATH "SkyBoxPS.cso", &renderResources->pPS);
	}
}
