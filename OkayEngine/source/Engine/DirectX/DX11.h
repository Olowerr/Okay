#pragma once
#include <d3d11.h>

#include "Engine/Okay/Okay.h"

class Window;

class DX11
{
private:
	DX11();
public:
	~DX11();
	DX11(const DX11&) = delete;
	DX11(DX11&&) = delete;
	DX11& operator=(const DX11&) = delete;

	static inline DX11& get()
	{
		static DX11 dx11;
		return dx11;
	}

	void shutdown();

	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();

	// Helper functions // TODO: Change UINT to uint32_t
	static bool createSwapChain(IDXGISwapChain** ppSwapChain, HWND hWnd, DXGI_USAGE dx11UsageFlags = DXGI_USAGE_RENDER_TARGET_OUTPUT);

	static HRESULT createVertexBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable = true);
	static HRESULT createIndexBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable = true);
	static HRESULT createConstantBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable = true);
	static void updateBuffer(ID3D11Resource* pBuffer, const void* pData, UINT byteSize);
	static void updateTexture(ID3D11Texture2D* pBuffer, const void* pData, uint32_t elementByteSize, uint32_t width = 0u, uint32_t height = 0u);

	static HRESULT createStructuredBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT eleByteSize, UINT numElements, bool immutable = true);
	static HRESULT createStructuredSRV(ID3D11ShaderResourceView** ppSRV, ID3D11Buffer* pBuffer, UINT numElements);

	static bool createVertexShader(std::string_view csoPath, ID3D11VertexShader** ppVertexShader);
	static bool createPixelShader(std::string_view csoPath, ID3D11PixelShader** ppVertexShader);


private:
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
};