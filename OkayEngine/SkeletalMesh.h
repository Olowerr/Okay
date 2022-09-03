#pragma once
#include "Okay.h"

namespace Okay
{
	struct TimeStamp
	{
		TimeStamp()
			:scale(1.f, 1.f, 1.f) { }

		float time = 0.f;
		Float3 pos;
		Float4 rot;
		Float3 scale;
	};

	struct Joint
	{
		std::string name;
		int parentIdx = -1;
		DirectX::XMMATRIX invBindPose{};
		DirectX::XMMATRIX localT{};
		DirectX::XMMATRIX modelT{};
		DirectX::XMMATRIX finalT{};
		std::vector<TimeStamp> stamps;
	};
	
	struct SkeletalVertexData
	{
		std::vector<Float3> position;
		std::vector<UVNormal> uvNormal;
		std::vector<UINT> indices;
		std::vector<SkinnedVertex> weights;
		std::vector<Joint> joints;
	};

	class SkeletalMesh
	{
	public:

		SkeletalMesh(const SkeletalVertexData& vertices);
		virtual ~SkeletalMesh();
		virtual void Shutdown();

		static const UINT NumBuffers = 3;
		static const UINT Stride[NumBuffers];
		static const UINT Offset[NumBuffers];

		void Draw() const;
		void DrawGeometry() const;

		std::vector<Joint>& GetJoints() { return joints; }

	private:
		ID3D11Buffer* vertexBuffer[NumBuffers];
		ID3D11Buffer* indexBuffer;

		const UINT numIndices;

		ID3D11Buffer* matricesBuffer;
		ID3D11ShaderResourceView* matricesSRV;
		std::vector<Joint> joints;

	public:
		SkeletalMesh(const SkeletalMesh&) = delete;
		SkeletalMesh& operator=(const SkeletalMesh&) = delete;
	};
}