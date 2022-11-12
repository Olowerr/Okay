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
		Okay::Float3 position	= Okay::Float3(0.f);
		Okay::Float3 rotation	= Okay::Float3(0.f);
		Okay::Float3 scale		= Okay::Float3(1.f);

		Okay::Float4x4 matrix = Okay::Float4x4(1.f);

		Okay::Float3 forward() const { return glm::normalize(matrix[2]); }
		Okay::Float3 up() const	  { return glm::normalize(matrix[1]); }
		Okay::Float3 right() const   { return glm::normalize(matrix[0]); }

		void calculateMatrix()
		{
			matrix = 
				Okay::mat4Translation(position) * 
				glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z) * 
				Okay::mat4Scale(scale);
		}
	};
}