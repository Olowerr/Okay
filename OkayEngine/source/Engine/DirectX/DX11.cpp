#include "DX11.h"
#include "Engine/Application/Window.h"

DX11::DX11()
	:pDevice(), pDeviceContext(), pSwapChain()
	, pBackBuffer(), pBackBufferRTV(), pBackBufferSRV()
	, pDepthBuffer(), pDepthBufferDSV()
{
	
}

DX11::~DX11()
{
	shutdown();
}

void DX11::initialize(Window* window)
{
	DX11& inst = DX11::getInstance();

	const uint32 Width = 0u;
	const uint32 Height = 0u;

	HRESULT hr{};
	DXGI_SWAP_CHAIN_DESC desc{};
	{
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.BufferCount = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;

		desc.BufferDesc.Width = Width;
		desc.BufferDesc.Height = Height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.RefreshRate.Numerator = 0U;
		desc.BufferDesc.RefreshRate.Denominator = 1U;

		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		desc.OutputWindow = window->getHWnd();
		desc.Windowed = true;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	}

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	UINT flags = 0;
#ifdef _DEBUG
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Device, DeviceContext, SwapChain
	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
		&featureLevel, 1, D3D11_SDK_VERSION, &desc, &inst.pSwapChain, &inst.pDevice, nullptr, &inst.pDeviceContext);
	if (FAILED(hr))
		return;

	// BackBuffer
	hr = inst.pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&inst.pBackBuffer);
	if (FAILED(hr))
		return;

	hr = inst.pDevice->CreateRenderTargetView(inst.pBackBuffer, nullptr, &inst.pBackBufferRTV);
	if (FAILED(hr))
		return;

	hr = inst.pDevice->CreateShaderResourceView(inst.pBackBuffer, nullptr, &inst.pBackBufferSRV);
	if (FAILED(hr))
		return;


	D3D11_TEXTURE2D_DESC backBufferDesc;
	inst.pBackBuffer->GetDesc(&backBufferDesc);

	// DepthBuffer
	D3D11_TEXTURE2D_DESC texDesc{};
	{
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.CPUAccessFlags = 0;
		texDesc.ArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Width = backBufferDesc.Width;
		texDesc.Height = backBufferDesc.Height;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.MiscFlags = 0;
	}

	hr = inst.pDevice->CreateTexture2D(&texDesc, nullptr, &inst.pDepthBuffer);
	if (FAILED(hr))
		return;

	hr = inst.pDevice->CreateDepthStencilView(inst.pDepthBuffer, nullptr, &inst.pDepthBufferDSV);
	if (FAILED(hr))
		return;
}

void DX11::shutdown()
{
	DX11_RELEASE(pBackBuffer);
	DX11_RELEASE(pBackBufferRTV);
	DX11_RELEASE(pBackBufferSRV);
		
	DX11_RELEASE(pDepthBuffer);
	DX11_RELEASE(pDepthBufferDSV);
	DX11_RELEASE(pDeviceContext);
		
	DX11_RELEASE(pSwapChain);
	DX11_RELEASE(pDevice);
}

void DX11::clear()
{
	static float clearColour[4] = { 0.6f, 0.2f, 0.8f, 1.f };

	pDeviceContext->ClearRenderTargetView(pBackBufferRTV, clearColour);
	pDeviceContext->ClearDepthStencilView(pDepthBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void DX11::present()
{
	pSwapChain->Present(0, 0);
}

ID3D11Device* DX11::getDevice()
{
	return pDevice;
}

ID3D11DeviceContext* DX11::getDeviceContext()
{
	return pDeviceContext;
}

IDXGISwapChain* DX11::getSwapChain()
{
	return pSwapChain;
}

ID3D11Texture2D* DX11::getBackBuffer()
{
	return pBackBuffer;
}

ID3D11RenderTargetView* const* DX11::getBackBufferRTV()
{
	return &pBackBufferRTV;
}

ID3D11ShaderResourceView* DX11::getBackBufferSRV()
{
	return pBackBufferSRV;
}

ID3D11Texture2D* DX11::getDepthBuffer()
{
	return pDepthBuffer;
}

ID3D11DepthStencilView* const* DX11::getDepthBufferDSV()
{
	return &pDepthBufferDSV;
}

bool DX11::resizeBackBuffer(uint32 width, uint32 height)
{
	DX11_RELEASE(pBackBuffer);
	DX11_RELEASE(pBackBufferRTV);
	DX11_RELEASE(pBackBufferSRV);

	pSwapChain->ResizeBuffers(1u, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	OKAY_VERIFY(pBackBuffer);

	pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pBackBufferRTV);
	OKAY_VERIFY(pBackBufferRTV);

	pDevice->CreateShaderResourceView(pBackBuffer, nullptr, &pBackBufferSRV);
	OKAY_VERIFY(pBackBufferSRV);

	OKAY_VERIFY(resizeDepthBuffer());

	return true;
}

bool DX11::resizeDepthBuffer()
{
	D3D11_TEXTURE2D_DESC desc;
	pBackBuffer->GetDesc(&desc);
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	DX11_RELEASE(pDepthBuffer);
	DX11_RELEASE(pDepthBufferDSV);

	pDevice->CreateTexture2D(&desc, nullptr, &pDepthBuffer);
	OKAY_VERIFY(pDepthBuffer);

	pDevice->CreateDepthStencilView(pDepthBuffer, nullptr, &pDepthBufferDSV);
	OKAY_VERIFY(pDepthBufferDSV);

	return true;
}




/* ------ HELPER FUNCTIONS ------ */

HRESULT DX11::createVertexBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable)
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
	return getInstance().pDevice->CreateBuffer(&desc, pData ? &inData : nullptr, ppBuffer);
}

HRESULT DX11::createIndexBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable)
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
	return getInstance().pDevice->CreateBuffer(&desc, pData ? &inData : nullptr, ppBuffer);
}

HRESULT DX11::createConstantBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable)
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
	return getInstance().pDevice->CreateBuffer(&desc, pData ? &inData : nullptr, ppBuffer);
}

bool DX11::updateBuffer(ID3D11Buffer* pBuffer, const void* pData, UINT byteSize)
{
	D3D11_MAPPED_SUBRESOURCE sub;
	OKAY_VERIFY(SUCCEEDED(getInstance().pDeviceContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub)));

	memcpy(sub.pData, pData, byteSize);
	getInstance().pDeviceContext->Unmap(pBuffer, 0);

	return true;
}

HRESULT DX11::createStructuredBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT eleByteSize, UINT numElements, bool immutable)
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = eleByteSize * numElements;
	desc.CPUAccessFlags = immutable ? 0 : D3D11_CPU_ACCESS_WRITE;
	desc.Usage = immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = eleByteSize;
	D3D11_SUBRESOURCE_DATA inData;
	inData.pSysMem = pData;
	inData.SysMemPitch = 0;
	inData.SysMemSlicePitch = 0;

	return getInstance().pDevice->CreateBuffer(&desc, pData ? &inData : nullptr, ppBuffer);
}
HRESULT DX11::createStructuredSRV(ID3D11ShaderResourceView** ppSRV, ID3D11Buffer* pBuffer, UINT numElements)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = numElements;

	return getInstance().pDevice->CreateShaderResourceView(pBuffer, &desc, ppSRV);
}