#pragma once
#include <memory>
#include "Engine/Graphics/Assets/Mesh.h"

namespace Okay
{
	struct MeshComponent
	{
		MeshComponent() = default;

		std::weak_ptr<Mesh> pMesh;
	};
}