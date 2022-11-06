#include "Mesh.h"


namespace Okay
{
	const uint32 Mesh::Stride[] = { sizeof(Float3), sizeof(Float2), sizeof(Float3) };
	const uint32 Mesh::Offset[] = { 0u, 0u, 0u };

	Mesh::Mesh(const VertexData& data, DX11* pDx11, std::string_view meshName)
		:numIndices((uint32)data.indices.size()), vertexBuffers{}, indexBuffer(), name(meshName)
	{
		pDx11->createIndexBuffer(&indexBuffer, data.indices.data(), sizeof(UINT)* (UINT)data.indices.size());

		pDx11->createVertexBuffer(&vertexBuffers[0], data.positions.data(), sizeof(Float3) * (uint32_t)data.positions.size());
		pDx11->createVertexBuffer(&vertexBuffers[1], data.uvs.data(),		sizeof(Float2) * (uint32_t)data.uvs.size());
		pDx11->createVertexBuffer(&vertexBuffers[1], data.normals.data(),	sizeof(Float3) * (uint32_t)data.normals.size());
	}

	Mesh::~Mesh()
	{
		shutdown();
	}

	void Mesh::shutdown()
	{
		DX11_RELEASE(vertexBuffers[0]);
		DX11_RELEASE(vertexBuffers[1]);
		DX11_RELEASE(vertexBuffers[2]);

		DX11_RELEASE(indexBuffer);
	}

	ID3D11Buffer* const* Mesh::getBuffers() const
	{
		return vertexBuffers;
	}

	ID3D11Buffer* Mesh::getIndexBuffer() const
	{
		return indexBuffer;
	}

	const std::string& Mesh::getName() const
	{
		return name;
	}

	void Mesh::setName(std::string_view name)
	{
		this->name = name;
	}
}
