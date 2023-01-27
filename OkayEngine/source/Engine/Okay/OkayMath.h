#pragma once
#include "glm/glm.hpp"

namespace Okay
{
	static glm::mat4 mat4Translation(const glm::vec3& translation)
	{
		return glm::mat4(
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			translation.x, translation.y, translation.z, 1.f);
	}

	static glm::mat4 mat4Scale(const glm::vec3& scale)
	{
		return glm::mat4(
			scale.x, 0.f, 0.f, 0.f,
			0.f, scale.y, 0.f, 0.f,
			0.f, 0.f, scale.z, 0.f,
			0.f, 0.f, 0.f, 1.f);
	}

	static float smoothStep(float x)
	{
		return x * x * x * (x * (6 * x - 15) + 10);
	}
}