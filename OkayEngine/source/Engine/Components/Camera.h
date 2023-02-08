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
		float fov;
		float nearZ;
		float farZ;

		Camera(float fov, float width, float height, float nearZ, float farZ)
			:fov(fov), nearZ(nearZ), farZ(farZ)
		{
			calculateProjMatrix(fov, width, height, nearZ, farZ);
		}

		Camera()
			:fov(90.f), nearZ(0.1f), farZ(10000.f)
		{
			calculateProjMatrix(fov, 16.f, 9.f, nearZ, farZ);
		}

		void calculateProjMatrix(float fov, float width, float height, float nearZ, float farZ)
		{
			this->fov = fov;
			this->nearZ = nearZ;
			this->farZ = farZ;

			projectionMatrix = glm::perspectiveFovLH(fov, width, height, nearZ, farZ);
			//projectionMatrix = glm::perspectiveFov(fov, width, height, nearZ, farZ);
		}

		void onTargetResize(float width, float height)
		{
			projectionMatrix = glm::perspectiveFovLH(fov, width, height, nearZ, farZ);
		}
	};
}