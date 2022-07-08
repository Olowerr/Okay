#include "Mesh.h"

Okay::Mesh::Mesh()
	:vertexBuffers{}, indexBuffer()
{
	// Triangle :]

	Float3 pos[3] = {
		{ 0.0f,	 0.5f, 0.0f},
		{-0.5f, -0.5f, 0.0f},
		{ 0.5f,	-0.5f, 0.0f}
	};

	UVNormal uvNormal[3]{};
	uvNormal[0].uv = { 0.5f, 0.f };
	uvNormal[1].uv = { 0.0f, 1.f };
	uvNormal[2].uv = { 1.0f, 1.f };

	uvNormal[0].normal = uvNormal[1].normal = uvNormal[2].normal = { 0.0f, 0.0f, 1.0f };

	UINT indices[3] = { 0,1,2 };

	DX11::CreateVertexBuffer(&vertexBuffers[0], pos, sizeof(pos));
	DX11::CreateVertexBuffer(&vertexBuffers[1], uvNormal, sizeof(uvNormal));
	DX11::CreateIndexBuffer(&indexBuffer, indices, sizeof(indices));
}

Okay::Mesh::Mesh(const std::string& filePath)
	:vertexBuffers{}, indexBuffer()
{
}

Okay::Mesh::~Mesh()
{
	Shutdown();
}

void Okay::Mesh::Shutdown()
{
	DX11_RELEASE(vertexBuffers[0]);
	DX11_RELEASE(vertexBuffers[1]);

	DX11_RELEASE(indexBuffer);
}

void Okay::Mesh::Bind() const
{
	DX11::Get().GetDeviceContext()->IASetVertexBuffers(0, NumBuffers, vertexBuffers, Stride, Offset);
	DX11::Get().GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Okay::Mesh::BindPosition() const
{
	DX11::Get().GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffers, Stride, Offset);
	DX11::Get().GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}
