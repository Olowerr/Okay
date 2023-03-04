#pragma once

#include "Engine/Okay/Okay.h"
#include "Engine/Graphics/SkyBox.h"

#include "glm/glm.hpp"

#include <memory>

namespace Okay
{
	struct SkyLight
	{
		SkyLight() = default;
		SkyLight(const glm::vec3& tint, float intensity)
			:tint(tint), intensity(intensity)
		{ }

		std::unique_ptr<SkyBox> skyBox = std::make_unique<SkyBox>();
		glm::vec3 tint = glm::vec3(1.f);
		float intensity = 0.2f;
	};
}