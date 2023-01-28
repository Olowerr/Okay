#pragma once

namespace Okay
{
	struct MeshComponent
	{
		MeshComponent() = default;
		MeshComponent(uint32_t meshIdx)
			:meshIdx(meshIdx)
		{
		}
		MeshComponent(uint32_t meshIdx, uint32_t materialIdx, uint32_t shaderIdx)
			:meshIdx(meshIdx), materialIdx(materialIdx), shaderIdx(shaderIdx)
		{
		}

		uint32_t meshIdx = 0u;
		uint32_t materialIdx = 0u;
		uint32_t shaderIdx = 0u;
	};
}