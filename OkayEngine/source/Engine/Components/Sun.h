#pragma once

#include "glm/glm.hpp"

namespace Okay
{
	struct Sun
	{
		Sun() = default;

		glm::vec3 colour = glm::vec3(1.f);
		float size = 50.f;
		float intensity = 1.f;
	};
}