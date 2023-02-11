#pragma once

#include "glm/glm.hpp"

namespace Okay
{
	struct PointLight
	{
		PointLight() = default;
		PointLight(const glm::vec3& colour, float intensity, const glm::vec2& attenuation)
			:colour(colour), intensity(intensity), attenuation(attenuation)
		{ }


		glm::vec3 colour = glm::vec3(1.f);
		float intensity = 1.f;
		glm::vec2 attenuation = glm::vec2(0.f, 0.1f); // TODO: Add value description
	};
}