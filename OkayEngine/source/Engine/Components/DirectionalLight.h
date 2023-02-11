#pragma once

#include "glm/glm.hpp"

namespace Okay
{
	struct DirectionalLight
	{
		DirectionalLight() = default;
		DirectionalLight(float intensity, const glm::vec3& colour)
			:intensity(intensity), colour(colour)
		{ }

		glm::vec3 colour = glm::vec3(1.f);
		float intensity = 1.f;
	};
}