#pragma once
#include "Okay/Okay.h"

namespace Okay
{
	struct TimeStamp
	{
		TimeStamp()
			:scale(1.f, 1.f, 1.f), pos(), rot() { }

		float time = 0.f;
		glm::vec3 pos;
		glm::vec4 rot;
		glm::vec3 scale;
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
		std::vector<glm::vec3> position;
		std::vector<UVNormal> uvNormal;
		std::vector<UINT> indices;
		std::vector<SkinnedVertex> weights;
		std::vector<Joint> joints;
		float durationS;
		float tickLengthS;
	};

	class SkeletalMesh
	{
	public:

		SkeletalMesh();
		SkeletalMesh(const SkeletalVertexData& data);
		virtual ~SkeletalMesh();
		virtual void Shutdown();

		static const UINT NumBuffers = 3;
		static const UINT Stride[NumBuffers];
		static const UINT Offset[NumBuffers];

		void Draw() const;
		void DrawGeometry() const;

		std::vector<Joint>& GetJoints() { return joints; }

		void SetPose(size_t frame);
		float GetDurationS()   const { return durationS;	 }
		float GetTickLengthS() const { return tickLengthS; }

	private:
		ID3D11Buffer* vertexBuffer[NumBuffers];
		ID3D11Buffer* indexBuffer;

		const UINT numIndices;

		ID3D11Buffer* matricesBuffer;
		ID3D11ShaderResourceView* matricesSRV;
		std::vector<Joint> joints;
		const float durationS;
		const float tickLengthS;

		std::vector<DirectX::XMFLOAT4X4> gpuMatrices;

	public:
		SkeletalMesh(const SkeletalMesh&) = delete;
		SkeletalMesh& operator=(const SkeletalMesh&) = delete;
	};
}