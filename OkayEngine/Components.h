#pragma once
#include "Mesh.h"
#include "Material.h"

namespace Okay // Structs for now, change to classes
{
	enum struct Components
	{
		None = 0,
		Mesh,
		Transform
	};

	struct CompMesh
	{
		static const Components ID = Components::Mesh;
		
		CompMesh();
		CompMesh(const Okay::String& meshName);

		std::shared_ptr<Mesh> mesh;


	};

	struct CompTransform
	{
		static const Components ID = Components::Transform;

		CompTransform();
		CompTransform(Float3 pos, Float3 rot, Float3 scale);

		DirectX::XMFLOAT4X4 matrix;
		Float3 position;
		Float3 rotation;
		Float3 scale;

		void CalcMatrix();
	};
}