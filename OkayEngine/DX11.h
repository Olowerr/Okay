#pragma once
#include "Okay.h"

class DX11
{
public:
	static DX11& Get()
	{
		static DX11 dx11;
		return dx11;
	}
	void Shutdown();

	void NewFrame();
	void EndFrame();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	IDXGISwapChain* GetSwapChain();

	ID3D11Texture2D* GetBackBuffer();
	ID3D11RenderTargetView* const* GetBackBufferRTV();

	ID3D11Texture2D* GetDepthBuffer();
	ID3D11DepthStencilView* const* GetDepthBufferDSV();

	// Helper functions
	static HRESULT CreateVertexBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable = true);
	static HRESULT CreateIndexBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable = true);
	static HRESULT CreateConstantBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable = true);
	static bool UpdateBuffer(ID3D11Buffer* pBuffer, const void* pData, UINT byteSize);

private:
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	IDXGISwapChain* pSwapChain;

	ID3D11Texture2D* pBackBuffer;
	ID3D11RenderTargetView* pBackBufferRTV;

	ID3D11Texture2D* pDepthBuffer;
	ID3D11DepthStencilView* pDepthBufferDSV;

private:
	DX11();
public:
	~DX11();
	DX11(const DX11&) = delete;
	DX11(DX11&&) = delete;
	DX11& operator=(const DX11&) = delete;
};