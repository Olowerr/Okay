#pragma once

#include "Okay.h"

namespace Okay
{
	class Mesh
	{
	public:
		Mesh();
		Mesh(const VertexData& vertices, const String& meshName);
		static int count;
		virtual ~Mesh();
		virtual void Shutdown();

		void Draw() const;			// Will change with instancing
		void DrawGeometry() const;	// Will change with instancing

		const String& GetName() const;
		void SetName(const Okay::String& name);

		static const UINT NumBuffers = 2;
		static const UINT Stride[NumBuffers];
		static const UINT Offset[NumBuffers];

	private:
		String name;
		ID3D11Buffer* vertexBuffers[NumBuffers];
		ID3D11Buffer* indexBuffer;

		const UINT numIndices;

	public:
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;
	};
	



}