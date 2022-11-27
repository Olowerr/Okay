#pragma once
#include <string>
#include <vector>

#include "Engine/Okay/Okay.h"
#include "Engine/DirectX/DX11.h"

namespace Okay
{
	class Mesh
	{
	public:
		struct MeshInfo
		{
			std::string name;

			std::vector<uint32> indices;

			std::vector<Okay::Float3> positions;
			std::vector<Okay::Float2> uvs;
			std::vector<Okay::Float3> normals;
			
			//std::vector<Okay::Float3> tangents;
			//std::vector<Okay::Float3> biTangents;
		};

		static const uint32 NumBuffers = 3;
		static const uint32 Stride[NumBuffers];
		static const uint32 Offset[NumBuffers];

		Mesh(const MeshInfo& data);
		~Mesh();
		Mesh& operator=(const Mesh&) = delete;

		void shutdown();

		ID3D11Buffer* const* getBuffers() const;
		ID3D11Buffer* getIndexBuffer() const;

		const std::string& getName() const;
		void setName(std::string_view name);

	private:
		std::string name;

		ID3D11Buffer* vertexBuffers[NumBuffers];
		ID3D11Buffer* indexBuffer;

		const uint32 numIndices;
	};
}