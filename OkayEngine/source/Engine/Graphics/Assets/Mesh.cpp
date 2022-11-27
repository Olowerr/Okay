#include "Mesh.h"
#include "Engine/DirectX/DX11.h"

namespace Okay
{
	const uint32 Mesh::Stride[] = { sizeof(Float3), sizeof(Float2), sizeof(Float3) };
	const uint32 Mesh::Offset[] = { 0u, 0u, 0u };

	Mesh::Mesh(const MeshInfo& data)
		:numIndices((uint32)data.indices.size()), vertexBuffers{}, indexBuffer(), name(data.name)
	{
		DX11& dx11 = DX11::getInstance();
		dx11.createIndexBuffer(&indexBuffer, data.indices.data(), uint32(sizeof(uint32) * data.indices.size()));

		dx11.createVertexBuffer(&vertexBuffers[0], data.positions.data(), uint32(sizeof(Float3) * data.positions.size()));
		dx11.createVertexBuffer(&vertexBuffers[1], data.uvs.data(),		  uint32(sizeof(Float2) * data.uvs.size()));
		dx11.createVertexBuffer(&vertexBuffers[1], data.normals.data(),	  uint32(sizeof(Float3) * data.normals.size()));
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
