#pragma once
#include "Engine/Okay/Okay.h"
#include "Engine/Okay/OkayMath.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

namespace Okay
{
	struct Transform
	{
		Transform() = default;
		glm::vec3 position	= glm::vec3(0.f);
		glm::vec3 rotation	= glm::vec3(0.f);
		glm::vec3 scale		= glm::vec3(1.f);

		glm::mat4 matrix = glm::mat4(1.f);

		glm::vec3 forward() const { return glm::normalize(glm::vec3(matrix[2])); }
		glm::vec3 up() const	  { return glm::normalize(glm::vec3(matrix[1])); }
		glm::vec3 right() const   { return glm::normalize(glm::vec3(matrix[0])); }

		void calculateMatrix()
		{
			matrix = 
				Okay::mat4Translation(position) * 
				glm::eulerAngleZYX(rotation.z, rotation.y, rotation.x) * 
				Okay::mat4Scale(scale);
		}
	};
}