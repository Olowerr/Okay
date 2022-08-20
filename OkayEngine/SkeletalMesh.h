#pragma once
#include "Okay.h"

namespace Okay
{
	class SkeletalMesh
	{
	public:

		struct VertexData
		{
			std::vector<Float3> position;
			std::vector<UVNormal> uvNormal;
			std::vector<UINT> indices;
			std::vector<SkinnedVertex> weights;
		};

		SkeletalMesh(const VertexData& vertices);
		virtual ~SkeletalMesh();
		virtual void Shutdown();

		static const UINT NumBuffers = 3;
		static const UINT Stride[NumBuffers];
		static const UINT Offset[NumBuffers];

	//private:
		ID3D11Buffer* vertexBuffer[NumBuffers];
		ID3D11Buffer* indexBuffer;

		const UINT numIndices;


	public:
		SkeletalMesh(const SkeletalMesh&) = delete;
		SkeletalMesh& operator=(const SkeletalMesh&) = delete;
	};
}