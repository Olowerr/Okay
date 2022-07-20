#include "DX11.h"

DX11::DX11()
	:pDevice(), pDeviceContext(), pSwapChain()
	, pBackBuffer(), pBackBufferRTV()
	, pDepthBuffer(), pDepthBufferDSV()
{
	HRESULT hr{};
	
	DXGI_SWAP_CHAIN_DESC desc{};
	{
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.BufferCount = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		desc.BufferDesc.Width = WIN_W;
		desc.BufferDesc.Height = WIN_H;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.RefreshRate.Numerator = 0U;
		desc.BufferDesc.RefreshRate.Denominator = 1U;

		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		desc.OutputWindow = GetHWindow();
		desc.Windowed = true;
		desc.Flags = 0;
	}
	
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	UINT flags = 0;
#ifdef _DEBUG
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Device, DeviceContext, SwapChain
	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
		&featureLevel, 1, D3D11_SDK_VERSION, &desc, &pSwapChain, &pDevice, nullptr, &pDeviceContext);
	if (FAILED(hr))
		return;
	

	// BackBuffer
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (FAILED(hr))
		return;

	hr = pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pBackBufferRTV);
	if (FAILED(hr))
		return;

	
	// DepthBuffer
	D3D11_TEXTURE2D_DESC dDesc{};
	{
		dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dDesc.Usage = D3D11_USAGE_DEFAULT;
		dDesc.CPUAccessFlags = 0;
		dDesc.ArraySize = 1;
		dDesc.MipLevels = 1;
		dDesc.Width = WIN_W;
		dDesc.Height= WIN_H;
		dDesc.SampleDesc.Count = 1;
		dDesc.SampleDesc.Quality = 0;
		dDesc.MiscFlags = 0;
	}
	
	hr = pDevice->CreateTexture2D(&dDesc, nullptr, &pDepthBuffer);
	if (FAILED(hr))
		return;

	hr = pDevice->CreateDepthStencilView(pDepthBuffer, nullptr, &pDepthBufferDSV);
	if (FAILED(hr))
		return;

}

DX11::~DX11()
{
	Shutdown();
}

void DX11::Shutdown()
{
	DX11_RELEASE(pDevice);
	DX11_RELEASE(pDeviceContext);
	DX11_RELEASE(pSwapChain);

	DX11_RELEASE(pBackBuffer);
	DX11_RELEASE(pBackBufferRTV);

	DX11_RELEASE(pDepthBuffer);
	DX11_RELEASE(pDepthBufferDSV);
}

void DX11::NewFrame()
{
	static float clearColour[4] = { 0.1f, 0.5f, 0.9f, 0.f };
	pDeviceContext->ClearRenderTargetView(pBackBufferRTV, clearColour);
	pDeviceContext->ClearDepthStencilView(pDepthBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void DX11::EndFrame()
{
	pSwapChain->Present(0, 0);
}

ID3D11Device* DX11::GetDevice()
{
	return pDevice;
}

ID3D11DeviceContext* DX11::GetDeviceContext()
{
	return pDeviceContext;
}

IDXGISwapChain* DX11::GetSwapChain()
{
	return pSwapChain;
}

ID3D11Texture2D* DX11::GetBackBuffer()
{
	return pBackBuffer;
}

ID3D11RenderTargetView* const* DX11::GetBackBufferRTV()
{
	return &pBackBufferRTV;
}

ID3D11Texture2D* DX11::GetDepthBuffer()
{
	return pDepthBuffer;
}

ID3D11DepthStencilView* const* DX11::GetDepthBufferDSV()
{
	return &pDepthBufferDSV;
}




/* ------ HELPER FUNCTIONS ------ */

HRESULT DX11::CreateVertexBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable)
{
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.Usage = immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC; 
	desc.CPUAccessFlags = immutable ? 0 : D3D11_CPU_ACCESS_WRITE;
	desc.ByteWidth = byteSize;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA inData{};
	inData.pSysMem = pData;
	inData.SysMemPitch = inData.SysMemSlicePitch = 0;
	return Get().GetDevice()->CreateBuffer(&desc, pData ? &inData : nullptr, ppBuffer);
}

HRESULT DX11::CreateIndexBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable)
{
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.Usage = immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = immutable ? 0 : D3D11_CPU_ACCESS_WRITE;
	desc.ByteWidth = byteSize;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA inData{};
	inData.pSysMem = pData;
	inData.SysMemPitch = inData.SysMemSlicePitch = 0;
	return Get().GetDevice()->CreateBuffer(&desc, pData ? &inData : nullptr, ppBuffer);
}

HRESULT DX11::CreateConstantBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable)
{
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = immutable ? 0 : D3D11_CPU_ACCESS_WRITE;
	desc.ByteWidth = byteSize;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA inData{};
	inData.pSysMem = pData;
	inData.SysMemPitch = inData.SysMemSlicePitch = 0;
	return Get().GetDevice()->CreateBuffer(&desc, pData ? &inData : nullptr, ppBuffer);
}

bool DX11::UpdateBuffer(ID3D11Buffer* pBuffer, const void* pData, UINT byteSize)
{
	D3D11_MAPPED_SUBRESOURCE sub;
	VERIFY_HR(Get().GetDeviceContext()->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub));

	memcpy(sub.pData, pData, byteSize);
	Get().GetDeviceContext()->Unmap(pBuffer, 0);

	return true;
}
