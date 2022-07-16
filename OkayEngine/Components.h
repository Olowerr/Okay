#pragma once
#include "ResourceManager.h"

namespace Okay // Structs for now, change to classes
{
	enum struct Components
	{
		None = 0,
		Mesh,
		Transform
	};

	struct MeshComponent
	{
		static const Components ID = Components::Mesh;
		
		MeshComponent() { mesh = Assets::GetMesh(""); foo(); }
		MeshComponent(const Okay::String& meshName) { Assets::GetMesh(meshName.c_str);foo(); }

		std::shared_ptr<Mesh> mesh;

		void foo() { }
	};

	struct TransformComponent
	{
		static const Components ID = Components::Transform;

		TransformComponent()
			:position(), rotation(), scale(1.f, 1.f, 1.f) { CalcMatrix(); }
		TransformComponent(Float3 pos, Float3 rot, Float3 scale)
			:position(pos), rotation(rot), scale(scale) { CalcMatrix(); }

		DirectX::XMFLOAT4X4 matrix;
		Float3 position;
		Float3 rotation;
		Float3 scale;

		void CalcMatrix()
		{
			using namespace DirectX;
			XMStoreFloat4x4(&matrix, XMMatrixTranspose(
				XMMatrixScaling(scale.x, scale.y, scale.z) *
				XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * 
				XMMatrixTranslation(position.x, position.y, position.z)
			));
		}
	};

	struct Qwe
	{
		Qwe() = default;
		int value = 0;
	};
}