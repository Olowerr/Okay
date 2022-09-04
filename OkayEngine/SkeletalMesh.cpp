#include "SkeletalMesh.h"
#include "DX11.h"


namespace Okay
{
	const UINT SkeletalMesh::Stride[] = { sizeof(Float3), sizeof(SkinnedVertex), sizeof(UVNormal)};
	const UINT SkeletalMesh::Offset[] = { 0,0,0 };

	SkeletalMesh::SkeletalMesh()
		:numIndices(0), durationS(0.f), tickLengthS(0.f)
	{
		vertexBuffer[0]	= nullptr;
		vertexBuffer[1]	= nullptr;
		vertexBuffer[2]	= nullptr;
		indexBuffer		= nullptr;
		matricesBuffer	= nullptr;
		matricesSRV		= nullptr;
	}

	SkeletalMesh::SkeletalMesh(const SkeletalVertexData& data)
		:numIndices((UINT)data.indices.size()), matricesBuffer(), matricesSRV()
		,durationS(data.durationS), tickLengthS(data.tickLengthS)
	{
		DX11& dx = DX11::Get();

		dx.CreateVertexBuffer(&vertexBuffer[0], data.position.data(), UINT( data.position.size() * sizeof(Float3)));
		dx.CreateVertexBuffer(&vertexBuffer[1], data.weights.data(),  UINT(data.weights.size()	 * sizeof(SkinnedVertex)));
		dx.CreateVertexBuffer(&vertexBuffer[2], data.uvNormal.data(), UINT( data.uvNormal.size() * sizeof(UVNormal)));
		dx.CreateIndexBuffer(&indexBuffer,		data.indices.data(),  UINT(data.indices.size()	 * sizeof(UINT)));
		

		joints.resize(data.joints.size());
		memcpy(joints.data(), data.joints.data(), sizeof(Joint) * joints.size());

		gpuMatrices.resize(data.joints.size());

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

	void SkeletalMesh::SetPose(size_t frame)
	{
		// ASSUMING ALL TIMESTAMPS ARE SYNCED AND SAME LENGTH

		if (frame >= joints[0].stamps.size())
			frame = joints[0].stamps.size() - 1;


		// OPTIMZE
		// OPTIMZE
		// OPTIMZE

		using namespace DirectX;
		TimeStamp& rootStamp = joints[0].stamps[frame];

		joints[0].localT =
			XMMatrixScaling(rootStamp.scale.x, rootStamp.scale.y, rootStamp.scale.z) *
			XMMatrixRotationQuaternion(XMVectorSet(rootStamp.rot.x, rootStamp.rot.y, rootStamp.rot.z, rootStamp.rot.w)) *
			XMMatrixTranslation(rootStamp.pos.x, rootStamp.pos.y, rootStamp.pos.z);

		joints[0].modelT = joints[0].localT;
		joints[0].finalT = joints[0].invBindPose * joints[0].modelT;

		for (size_t i = 1; i < joints.size(); i++)
		{
			TimeStamp& stamp = joints[i].stamps.size() > 1 ? joints[i].stamps[frame] : joints[i].stamps[0];

			joints[i].localT =
				XMMatrixScaling(stamp.scale.x, stamp.scale.y, stamp.scale.z) *
				XMMatrixRotationQuaternion(XMVectorSet(stamp.rot.x, stamp.rot.y, stamp.rot.z, stamp.rot.w)) *
				XMMatrixTranslation(stamp.pos.x, stamp.pos.y, stamp.pos.z);

			joints[i].modelT = joints[i].localT * joints[joints[i].parentIdx].modelT;
			joints[i].finalT = joints[i].invBindPose * joints[i].modelT;

		}

		for (size_t i = 0; i < joints.size(); i++)
			XMStoreFloat4x4(&gpuMatrices[i], XMMatrixTranspose(joints[i].finalT));
	}

}
