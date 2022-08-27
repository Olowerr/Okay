#include "SkeletalMesh.h"
#include "DX11.h"


namespace Okay
{
	const UINT SkeletalMesh::Stride[] = { sizeof(Float3), sizeof(SkinnedVertex), sizeof(UVNormal)};
	const UINT SkeletalMesh::Offset[] = { 0,0,0 };

	SkeletalMesh::SkeletalMesh(const SkeletalVertexData& vertices)
		:numIndices((UINT)vertices.indices.size()), matricesBuffer(), matricesSRV()
	{
		DX11& dx = DX11::Get();

		dx.CreateVertexBuffer(&vertexBuffer[0], vertices.position.data(),UINT( vertices.position.size() * sizeof(Float3)));
		dx.CreateVertexBuffer(&vertexBuffer[1], vertices.weights.data(), UINT(vertices.weights.size() * sizeof(SkinnedVertex)));
		dx.CreateVertexBuffer(&vertexBuffer[2], vertices.uvNormal.data(),UINT( vertices.uvNormal.size() * sizeof(UVNormal)));
		dx.CreateIndexBuffer(&indexBuffer, vertices.indices.data(), UINT(vertices.indices.size() * sizeof(UINT)));
		

		joints.resize(vertices.joints.size());
		memcpy(joints.data(), vertices.joints.data(), sizeof(Joint) * joints.size());

		DX11::CreateStructuredBuffer(&matricesBuffer, nullptr, 64U, joints.size(), false);
		DX11::CreateStructuredSRV(&matricesSRV, matricesBuffer, joints.size());
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

		DX11_RELEASE(matricesBuffer);
		DX11_RELEASE(matricesSRV);
	}

	void SkeletalMesh::Draw() const
	{
		ID3D11DeviceContext* dc = DX11::Get().GetDeviceContext();
		dc->IASetVertexBuffers(0, NumBuffers, vertexBuffer, Stride, Offset);
		dc->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dc->VSSetShaderResources(0, 1, &matricesSRV);
		dc->DrawIndexed(numIndices, 0, 0);
		
	}

	void SkeletalMesh::DrawGeometry() const
	{
		ID3D11DeviceContext* dc = DX11::Get().GetDeviceContext();
		dc->IASetVertexBuffers(0, 2, vertexBuffer, Stride, Offset);
		dc->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dc->VSSetShaderResources(0, 1, &matricesSRV);
		dc->DrawIndexed(numIndices, 0, 0);
	}

}
