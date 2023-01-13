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

			std::vector<uint32_t> indices;

			std::vector<glm::vec3> positions;
			std::vector<glm::vec2> uvs;
			std::vector<glm::vec3> normals;
			
			//std::vector<glm::vec3> tangents;
			//std::vector<glm::vec3> biTangents;
		};

		static const uint32_t NumBuffers = 3;
		static const uint32_t Stride[NumBuffers];
		static const uint32_t Offset[NumBuffers];

		Mesh();
		Mesh(const MeshInfo& data);
		Mesh(Mesh&& other) noexcept;
		~Mesh();
		//Mesh& operator=(const Mesh&) = delete;
		//Mesh(const Mesh&) = delete;

		void create(const MeshInfo& data);

		void shutdown();

		ID3D11Buffer* const* getBuffers() const;
		ID3D11Buffer** getBuffers();

		ID3D11Buffer* getIndexBuffer() const;
		uint32_t getNumIndices() const;

		const std::string& getName() const;
		void setName(std::string_view name);

	private:
		std::string name;

		ID3D11Buffer* vertexBuffers[NumBuffers];
		ID3D11Buffer* indexBuffer;

		uint32_t numIndices;
	};
}