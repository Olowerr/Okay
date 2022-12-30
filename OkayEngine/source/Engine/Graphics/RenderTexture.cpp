#include "RenderTexture.h"
#include "Engine/DirectX/DX11.h"

namespace Okay
{
	RenderTexture::RenderTexture()
		:buffer(nullptr), rtv(nullptr), srv(nullptr), uav(nullptr),
		depthBuffer(nullptr), dsv(nullptr)
	{
	}

	RenderTexture::RenderTexture(ID3D11Texture2D* texture, uint32_t flags)
		:buffer(nullptr), rtv(nullptr), srv(nullptr), uav(nullptr),
		depthBuffer(nullptr), dsv(nullptr)
	{
		create(texture, flags);
	}

	RenderTexture::RenderTexture(uint32_t width, uint32_t height, uint32_t flags)
		:buffer(nullptr), rtv(nullptr), srv(nullptr), uav(nullptr),
		depthBuffer(nullptr), dsv(nullptr)
	{
		create(width, height, flags);
	}

	RenderTexture::~RenderTexture()
	{
		shutdown();
	}

	void RenderTexture::shutdown()
	{
		DX11_RELEASE(buffer);
		DX11_RELEASE(rtv);
		DX11_RELEASE(srv);
		DX11_RELEASE(uav);

		DX11_RELEASE(depthBuffer);
		DX11_RELEASE(dsv);
	}
	
	void RenderTexture::create(ID3D11Texture2D* texture, uint32_t flags)
	{
		texture->AddRef();
		buffer = texture;

		readFlgs(flags);
	}

	void RenderTexture::create(uint32_t width, uint32_t height, uint32_t flags)
	{
		shutdown();

		ID3D11Device* pDevice = DX11::getInstance().getDevice();

		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = width;
		desc.Height = height;
		desc.ArraySize = 1u;
		desc.CPUAccessFlags = 0u;
		desc.MipLevels = 1u;
		desc.MiscFlags = 0u;
		desc.SampleDesc.Count = 1u;
		desc.SampleDesc.Quality = 0u;
		desc.Usage = D3D11_USAGE_DEFAULT;

		desc.Format = CHECK_BIT(flags, BitPos::B_BIT_32) ?
			DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BindFlags =
			(CHECK_BIT(flags, BitPos::B_RENDER) ? D3D11_BIND_RENDER_TARGET : 0u) |
			(CHECK_BIT(flags, BitPos::B_SHADER_READ) ? D3D11_BIND_SHADER_RESOURCE : 0u) |
			(CHECK_BIT(flags, BitPos::B_SHADER_WRITE) ? D3D11_BIND_UNORDERED_ACCESS : 0u);

		pDevice->CreateTexture2D(&desc, nullptr, &buffer);
		OKAY_ASSERT(buffer, "Failed creating RenderTexture");

		readFlgs(flags);
	}

	void RenderTexture::clear()
	{
		static float white[4]{ 0.f, 0.f, 0.f, 1.f };
		DX11::getInstance().getDeviceContext()->ClearRenderTargetView(rtv, white);

		if (dsv)
			DX11::getInstance().getDeviceContext()->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.f, 0);
	}

	void RenderTexture::clear(float* colour)
	{
		DX11::getInstance().getDeviceContext()->ClearRenderTargetView(rtv, colour);

		if (dsv)
			DX11::getInstance().getDeviceContext()->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.f, 0);
	}

	void RenderTexture::clear(const glm::vec4& colour)
	{
		DX11::getInstance().getDeviceContext()->ClearRenderTargetView(rtv, &colour.r);

		if (dsv)
			DX11::getInstance().getDeviceContext()->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.f, 0);

	}

	glm::ivec2 RenderTexture::getDimensions() const
	{
		D3D11_TEXTURE2D_DESC desc;
		buffer->GetDesc(&desc);
		return glm::ivec2((int)desc.Width, (int)desc.Height);
	}

	void RenderTexture::readFlgs(uint32_t flags)
	{
		ID3D11Device* pDevice = DX11::getInstance().getDevice();

		if (CHECK_BIT(flags, BitPos::B_RENDER))
		{
			pDevice->CreateRenderTargetView(buffer, nullptr, &rtv);
			OKAY_ASSERT(rtv, "Failed creating RTV");
		}
		if (CHECK_BIT(flags, BitPos::B_SHADER_READ))
		{
			pDevice->CreateShaderResourceView(buffer, nullptr, &srv);
			OKAY_ASSERT(srv, "Failed creating SRV");
		}
		if (CHECK_BIT(flags, BitPos::B_SHADER_WRITE))
		{
			pDevice->CreateUnorderedAccessView(buffer, nullptr, &uav);
			OKAY_ASSERT(uav, "Failed creating UAV");
		}
		if (CHECK_BIT(flags, BitPos::B_DEPTH))
		{
			D3D11_TEXTURE2D_DESC desc;
			buffer->GetDesc(&desc);

			desc.Format = DXGI_FORMAT_D32_FLOAT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

			pDevice->CreateTexture2D(&desc, nullptr, &depthBuffer);
			OKAY_ASSERT(buffer, "Failed creating DepthBuffer");

			pDevice->CreateDepthStencilView(depthBuffer, nullptr, &dsv);
			OKAY_ASSERT(dsv, "Failed creating DSV");
		}
	}
}
