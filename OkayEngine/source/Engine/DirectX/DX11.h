#pragma once
#include <d3d11.h>
#include "Engine/Okay/OkayDefine.h"

class Window;

class DX11
{
public:
	DX11();
	virtual ~DX11();

	DX11(const DX11&) = delete;
	DX11(DX11&&) = delete;
	DX11& operator=(const DX11&) = delete;

	void initalize(Window* window);
	void shutdown();

	void clear();
	void present();

	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
	IDXGISwapChain* getSwapChain();

	ID3D11Texture2D* getBackBuffer();
	ID3D11RenderTargetView* const* getBackBufferRTV();
	ID3D11ShaderResourceView* getBackBufferSRV();

	ID3D11Texture2D* getDepthBuffer();
	ID3D11DepthStencilView* const* getDepthBufferDSV();

	bool resizeBackBuffer(uint32 width, uint32 height);

	// Helper functions
	HRESULT createVertexBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable = true);
	HRESULT createIndexBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable = true);
	HRESULT createConstantBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable = true);
	bool updateBuffer(ID3D11Buffer* pBuffer, const void* pData, UINT byteSize);

	HRESULT createStructuredBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT eleByteSize, UINT numElements, bool immutable = true);
	HRESULT createStructuredSRV(ID3D11ShaderResourceView** ppSRV, ID3D11Buffer* pBuffer, UINT numElements);

private:
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	IDXGISwapChain* pSwapChain;

	ID3D11Texture2D* pBackBuffer;
	ID3D11RenderTargetView* pBackBufferRTV;
	ID3D11ShaderResourceView* pBackBufferSRV;

	ID3D11Texture2D* pDepthBuffer;
	ID3D11DepthStencilView* pDepthBufferDSV;

	bool resizeDepthBuffer();

public:
	
};