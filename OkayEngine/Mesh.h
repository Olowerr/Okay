#pragma once

#include "DX11.h"

namespace Okay
{
	class Mesh
	{
	public:
		Mesh();
		//Mesh(const std::string& filePath);
		Mesh(const VertexData& vertices, const String& meshName);

		virtual ~Mesh();
		virtual void Shutdown();

		void Bind() const;			// Will change with instancing
		void BindPosition() const;	// Will change with instancing
		void Draw() const;			// Will change with instancing

		static const UINT NumBuffers = 2;
		static const UINT Stride[2];
		static const UINT Offset[2];

	private:
		const String name;
		ID3D11Buffer* vertexBuffers[NumBuffers];
		ID3D11Buffer* indexBuffer;

		const UINT numIndices;

	public:
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;
	};
	



}