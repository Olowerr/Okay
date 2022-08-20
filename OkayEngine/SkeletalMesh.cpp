#include "SkeletalMesh.h"
#include "DX11.h"


namespace Okay
{
	const UINT SkeletalMesh::Stride[] = { sizeof(Float3), sizeof(UVNormal), sizeof(SkinnedVertex)};
	const UINT SkeletalMesh::Offset[] = { 0,0,0 };

	SkeletalMesh::SkeletalMesh(const VertexData& vertices)
		:numIndices((UINT)vertices.indices.size())
	{
		DX11& dx = DX11::Get();
		
		dx.CreateVertexBuffer(&vertexBuffer[0], vertices.position.data(),UINT( vertices.position.size() * sizeof(Float3)));
		dx.CreateVertexBuffer(&vertexBuffer[1], vertices.uvNormal.data(),UINT( vertices.uvNormal.size() * sizeof(UVNormal)));
		dx.CreateVertexBuffer(&vertexBuffer[2], vertices.weights.data(), UINT(vertices.weights.size() * sizeof(SkinnedVertex)));
		dx.CreateIndexBuffer(&indexBuffer, vertices.indices.data(), UINT(vertices.indices.size() * sizeof(UINT)));
	}

	SkeletalMesh::~SkeletalMesh()
	{
		Shutdown();
	}

	void SkeletalMesh::Shutdown()
	{
		DX11_RELEASE(vertexBuffer[0]);
		DX11_RELEASE(vertexBuffer[1]);
		DX11_RELEASE(vertexBuffer[2]);
		DX11_RELEASE(indexBuffer);
	}

}
