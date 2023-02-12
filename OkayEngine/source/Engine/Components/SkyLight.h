#pragma once

#include "Engine/Okay/Okay.h"

#include "glm/glm.hpp"

namespace Okay
{
	struct SkyLight
	{
		SkyLight() = default;
		SkyLight(uint32_t skyBoxIdx, const glm::vec3& tint, float intensity)
			:skyBoxIdx(skyBoxIdx), tint(tint), intensity(intensity)
		{ }

		uint32_t skyBoxIdx = INVALID_UINT;
		glm::vec3 tint = glm::vec3(1.f);
		float intensity = 1.f;
	};
}