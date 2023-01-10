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

	static inline DX11& getInstance()
	{
		static DX11 dx11;
		return dx11;
	}

	void shutdown();

	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();

	// Helper functions
	static HRESULT createVertexBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable = true);
	static HRESULT createIndexBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable = true);
	static HRESULT createConstantBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT byteSize, bool immutable = true);
	static bool updateBuffer(ID3D11Resource* pBuffer, const void* pData, UINT byteSize);

	static HRESULT createStructuredBuffer(ID3D11Buffer** ppBuffer, const void* pData, UINT eleByteSize, UINT numElements, bool immutable = true);
	static HRESULT createStructuredSRV(ID3D11ShaderResourceView** ppSRV, ID3D11Buffer* pBuffer, UINT numElements);

private:
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
};