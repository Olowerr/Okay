#pragma once

#include "Engine/Okay/Okay.h"
#include "Engine/Graphics/SkyBox.h"

#include "glm/glm.hpp"

#include <memory>

namespace Okay
{
	struct SkyLight
	{
		SkyLight() = default;

		// TODO: Change to shared_ptr ? 
		std::unique_ptr<SkyBox> skyBox = std::make_unique<SkyBox>();

		glm::vec3 ambientTint = glm::vec3(1.f);
		float ambientIntensity = 0.2f;

		glm::vec3 sunColour = glm::vec3(1.f);
		float sunSize = 50.f; // Subject to change
		float sunIntensity = 1.f;
	};
}