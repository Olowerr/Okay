#include "DX11.h"
#include "Engine/Application/Window.h"

DX11::DX11()
	:pDevice(), pDeviceContext()
{
	const uint32_t Width = 0u;
	const uint32_t Height = 0u;

	HRESULT hr{};
	
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	uint32_t flags = 0;
#ifndef DIST
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Device and DeviceContext
	hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
		&featureLevel, 1u, D3D11_SDK_VERSION, &pDevice, nullptr, &pDeviceContext);
	OKAY_ASSERT(SUCCEEDED(hr), "Failed creating instantiating DX11");
}

DX11::~DX11()
{
	shutdown();
}

void DX11::shutdown()
{
	DX11_RELEASE(pDeviceContext);

#ifndef DIST
	ID3D11Debug* debugger = nullptr;
	pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugger));
	debugger->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	debugger->Release();
#endif

	DX11_RELEASE(pDevice);
}

ID3D11Device* DX11::getDevice()
{
	return pDevice;
}

ID3D11DeviceContext* DX11::getDeviceContext()
{
	return pDeviceContext;
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