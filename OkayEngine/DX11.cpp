#include "DX11.h"

DX11::DX11()
	:pDevice(), pDeviceContext(), pSwapChain()
	, pBackBuffer(), pBackBufferRTV()
	, pDepthBuffer(), pDepthBufferDSV()
	, winWidth(WIN_W), winHeight(WIN_H)
	, mainWidth(winWidth), mainHeight(winHeight)
{
	HRESULT hr{};

	DXGI_SWAP_CHAIN_DESC desc{};
	{
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.BufferCount = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;

		desc.BufferDesc.Width = winWidth;
		desc.BufferDesc.Height = winHeight;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.RefreshRate.Numerator = 0U;
		desc.BufferDesc.RefreshRate.Denominator = 1U;

		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		desc.OutputWindow = GetHWindow();
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
	
	hr = pDevice->CreateShaderResourceView(pBackBuffer, nullptr, &pBackBufferSRV);
	if (FAILED(hr))
		return;

	// DepthBuffer
	D3D11_TEXTURE2D_DESC texDesc{};
	{
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.CPUAccessFlags = 0;
		texDesc.ArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Width = winWidth;
		texDesc.Height = winHeight;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.MiscFlags = 0;
	}

	hr = pDevice->CreateTexture2D(&texDesc, nullptr, &pDepthBuffer);
	if (FAILED(hr))
		return;

	hr = pDevice->CreateDepthStencilView(pDepthBuffer, nullptr, &pDepthBufferDSV);
	if (FAILED(hr))
		return;


	// MainBuffer
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	hr = pDevice->CreateTexture2D(&texDesc, nullptr, &pMainBuffer);
	if (FAILED(hr))
		return;

	hr = pDevice->CreateRenderTargetView(pMainBuffer, nullptr, &pMainRTV);
	if (FAILED(hr))
		return;

	hr = pDevice->CreateShaderResourceView(pMainBuffer, nullptr, &pMainSRV);
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
	DX11_RELEASE(pBackBufferSRV);

	DX11_RELEASE(pMainBuffer);
	DX11_RELEASE(pMainRTV);
	DX11_RELEASE(pMainSRV);

	DX11_RELEASE(pDepthBuffer);
	DX11_RELEASE(pDepthBufferDSV);
}

void DX11::NewFrame()
{
	static float clearColour[4] = { 0.1f, 0.5f, 0.9f, 1.f };
	pDeviceContext->ClearRenderTargetView(pBackBufferRTV, clearColour);
	pDeviceContext->ClearRenderTargetView(pMainRTV, clearColour);
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

ID3D11ShaderResourceView* DX11::GetBackBufferSRV()
{
	return pBackBufferSRV;
}

ID3D11Texture2D* DX11::GetMainBuffer()
{
	return pMainBuffer;
}

ID3D11RenderTargetView* const* DX11::GetMainRTV()
{
	return &pMainRTV;
}

ID3D11ShaderResourceView* const* DX11::GetMainSRV()
{
	return &pMainSRV;
}

ID3D11Texture2D* DX11::GetDepthBuffer()
{
	return pDepthBuffer;
}

ID3D11DepthStencilView* const* DX11::GetDepthBufferDSV()
{
	return &pDepthBufferDSV;
}

bool DX11::ResizeBackBuffer()
{
	//printf("Resized BackBuffer\n");

	DX11_RELEASE(pBackBuffer);
	DX11_RELEASE(pBackBufferRTV);
	DX11_RELEASE(pBackBufferSRV);

	pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	VERIFY(pBackBuffer);

	pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pBackBufferRTV);
	VERIFY(pBackBufferRTV);

	pDevice->CreateShaderResourceView(pBackBuffer, nullptr, &pBackBufferSRV);
	VERIFY(pBackBufferSRV);


	//ResizeDepthBuffer(pBackBuffer);

	return true;
}

bool DX11::ResizeMainBuffer(UINT width, UINT height)
{
	//printf("Resized MainBuffer\n");

	DX11_RELEASE(pMainBuffer);
	DX11_RELEASE(pMainRTV);
	DX11_RELEASE(pMainSRV);

	D3D11_TEXTURE2D_DESC desc;
	pBackBuffer->GetDesc(&desc);
	desc.Width = mainWidth = width;
	desc.Height = mainHeight = height;
	
	pDevice->CreateTexture2D(&desc, nullptr, &pMainBuffer);
	VERIFY(pMainBuffer);

	pDevice->CreateShaderResourceView(pMainBuffer, nullptr, &pMainSRV);
	VERIFY(pMainSRV);

	pDevice->CreateRenderTargetView(pMainBuffer, nullptr, &pMainRTV);
	VERIFY(pMainRTV);

	ResizeDepthBuffer(pMainBuffer);

	return true;
}

bool DX11::ResizeDepthBuffer(ID3D11Texture2D* ref)
{
	DX11_RELEASE(pDepthBuffer);
	DX11_RELEASE(pDepthBufferDSV);

	D3D11_TEXTURE2D_DESC desc;
	ref->GetDesc(&desc);
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	pDevice->CreateTexture2D(&desc, nullptr, &pDepthBuffer);
	VERIFY(pDepthBuffer);

	pDevice->CreateDepthStencilView(pDepthBuffer, nullptr, &pDepthBufferDSV);
	VERIFY(pDepthBufferDSV);

	return true;
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
