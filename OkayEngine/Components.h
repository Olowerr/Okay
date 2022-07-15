#pragma once
#include "Mesh.h"

namespace Okay // Structs for now, change to classes
{
	enum struct Components
	{
		Mesh,
		Transform
	};

	struct MeshComponent
	{
		static const Components ID = Components::Mesh;

		std::shared_ptr<Mesh> mesh;

		
	};

	struct TransformComponent
	{
		static const Components ID = Components::Transform;

		DirectX::XMFLOAT4X4 matrix;
		void SetPosition(Float3 pos)
		{
			DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixTranspose(
				DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z)
			));
		}
	};
}