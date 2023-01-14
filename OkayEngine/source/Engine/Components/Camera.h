#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Okay
{
	struct Camera
	{
		// TODO: Add support for target point camera
		// without forcing the calculation on the user

		glm::mat4 projectionMatrix;

		Camera(float fov, float width, float height, float nearZ, float farZ)
		{
			calculateProjMatrix(fov, width, height, nearZ, farZ);
		}

		Camera()
		{
			calculateProjMatrix(90.f, 16.f, 9.f, 0.1f, 10000.f);
		}

		void calculateProjMatrix(float fov, float width, float height, float nearZ, float farZ)
		{
			projectionMatrix = glm::perspectiveFovLH(fov, width, height, nearZ, farZ);
			//projectionMatrix = glm::perspectiveFov(fov, width, height, nearZ, farZ);
		}

	};
}