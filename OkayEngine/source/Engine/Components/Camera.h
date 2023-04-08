#pragma once

#include "glm/gtc/matrix_transform.hpp"

namespace Okay
{
	struct Camera
	{
		// TODO: Add support for target point camera
		// without forcing the calculation on the user

		float fov = glm::radians(90.f);
		float nearZ = 0.1f;
		float farZ = 1000.f;

		Camera() = default;

		Camera(float fov, float nearZ, float farZ)
			:fov(fov), nearZ(nearZ), farZ(farZ)
		{
		}

		inline glm::mat4 calculateProjMatrix(float width, float height) const
		{
			return glm::perspectiveFovLH(fov, width, height, nearZ, farZ);
			//projectionMatrix = glm::perspectiveFov(fov, width, height, nearZ, farZ);
		}
	};
}