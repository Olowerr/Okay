#pragma once
#include "Engine/Okay/Okay.h"
#include "Engine/Okay/OkayMath.h"

#include "glm/gtx/quaternion.hpp"

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
				glm::toMat4(glm::quat(rotation)) * 
				Okay::mat4Scale(scale);
		}
	};
}