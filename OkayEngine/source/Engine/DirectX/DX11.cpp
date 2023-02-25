#include "DX11.h"
#include "Engine/Application/Window.h"

DX11::DX11()
	:pDevice(), pDeviceContext()
{
	HRESULT hr{};
	
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	uint32_t flags = 0;
#ifndef DIST	
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Device and DeviceContext
	hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
		&featureLevel, 1u, D3D11_SDK_VERSION, &pDevice, nullptr, &pDeviceContext);
	OKAY_ASSERT(SUCCEEDED(hr), "Failed initializing DirectX 11");
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
	return get().pDevice->CreateBuffer(&desc, pData ? &inData : nullptr, ppBuffer);
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
	return get().pDevice->CreateBuffer(&desc, pData ? &inData : nullptr, ppBuffer);
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
	return get().pDevice->CreateBuffer(&desc, pData ? &inData : nullptr, ppBuffer);
}

void DX11::updateBuffer(ID3D11Resource* pBuffer, const void* pData, UINT byteSize)
{
	D3D11_MAPPED_SUBRESOURCE sub;
	if (FAILED(get().pDeviceContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub)))
		return;

	memcpy(sub.pData, pData, byteSize);
	get().pDeviceContext->Unmap(pBuffer, 0);
}

void DX11::updateTexture(ID3D11Texture2D* pBuffer, const void* pData, uint32_t elementByteSize, uint32_t width, uint32_t height)
{
	D3D11_BOX box{};
	box.left = 0u;
	box.top = 0u;
	box.front = 0u;
	box.back = 1u;

	if (!width || !height)
	{
		D3D11_TEXTURE2D_DESC desc{};
		pBuffer->GetDesc(&desc);
		width = width ? width : desc.Width;
		height = height ? height : desc.Height;
	}
	box.right = width;
	box.bottom = height;

	get().pDeviceContext->UpdateSubresource(pBuffer, 0, &box, pData, width * elementByteSize, 0u);
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

	return get().pDevice->CreateBuffer(&desc, pData ? &inData : nullptr, ppBuffer);
}

HRESULT DX11::createStructuredSRV(ID3D11ShaderResourceView** ppSRV, ID3D11Buffer* pBuffer, UINT numElements)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = numElements;

	return get().pDevice->CreateShaderResourceView(pBuffer, &desc, ppSRV);
}

bool DX11::createVertexShader(std::string_view csoPath, ID3D11VertexShader** ppVertexShader)
{
	std::string shaderData;
	bool result = Okay::readBinary(csoPath, shaderData);
	OKAY_ASSERT(result, "Failed reading Vertex Shader CSO");

	HRESULT hr = get().pDevice->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, ppVertexShader);
	OKAY_ASSERT(SUCCEEDED(hr), "Failed creating Vertex Shader");

	return true;
}

bool DX11::createPixelShader(std::string_view csoPath, ID3D11PixelShader** ppPixelShader)
{
	std::string shaderData;
	bool result = Okay::readBinary(csoPath, shaderData);
	OKAY_ASSERT(result, "Failed reading Pixel Shader CSO");

	HRESULT hr = get().pDevice->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, ppPixelShader);
	OKAY_ASSERT(SUCCEEDED(hr), "Failed creating Pixel Shader");

	return true;
}
