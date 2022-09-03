#include "Mesh.h"
#include "DX11.h"

const UINT Okay::Mesh::Stride[] = { sizeof(Float3), sizeof(UVNormal) };
const UINT Okay::Mesh::Offset[] = { 0,0 };
int Okay::Mesh::count = 0;

Okay::Mesh::Mesh()
	:vertexBuffers{}, indexBuffer(), numIndices(3), name("Default")
{
	// Triangle :]
	count++;
	Float3 pos[3] = {
		{ 0.0f,	  0.5f, 0.0f},
		{ 0.5f,  -0.5f, 0.0f},
		{ -0.5f, -0.5f, 0.0f}
	};

	UVNormal uvNormal[3]{};
	uvNormal[0].uv = { 0.5f, 0.f };
	uvNormal[1].uv = { 1.0f, 1.f };
	uvNormal[2].uv = { 0.0f, 1.f };

	uvNormal[0].normal = uvNormal[1].normal = uvNormal[2].normal = { 0.0f, 0.0f, -1.0f };

	UINT indices[3] = { 0,1,2 };

	DX11::CreateVertexBuffer(&vertexBuffers[0], pos, sizeof(pos));
	DX11::CreateVertexBuffer(&vertexBuffers[1], uvNormal, sizeof(uvNormal));
	DX11::CreateIndexBuffer(&indexBuffer, indices, sizeof(indices));
}

Okay::Mesh::Mesh(const VertexData& vertices, const String& meshName)
	:numIndices((UINT)vertices.indices.size()), vertexBuffers{}, indexBuffer(), name(meshName)
{
	count++;
	DX11::CreateVertexBuffer(&vertexBuffers[0], vertices.position.data(), sizeof(Float3)* (UINT)vertices.position.size());
	DX11::CreateVertexBuffer(&vertexBuffers[1], vertices.uvNormal.data(), sizeof(UVNormal)* (UINT)vertices.uvNormal.size());
	DX11::CreateIndexBuffer(&indexBuffer, vertices.indices.data(), sizeof(UINT)* (UINT)vertices.indices.size());
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

void Okay::Mesh::Draw() const
{
	DX11::Get().GetDeviceContext()->DrawIndexed(numIndices, 0, 0);
}

const Okay::String& Okay::Mesh::GetName() const
{
	return name;
}

void Okay::Mesh::SetName(const Okay::String& name)
{
	this->name = name;
}
