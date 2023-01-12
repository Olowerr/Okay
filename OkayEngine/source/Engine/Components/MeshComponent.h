#pragma once
#include "Engine/Graphics/Assets/Mesh.h"

namespace Okay
{
	struct MeshComponent
	{
		MeshComponent() = default;

		uint32_t meshIdx = 0u;
		uint32_t materialIdx = 0u;
		uint32_t shaderIdx = 0u;
	};
}