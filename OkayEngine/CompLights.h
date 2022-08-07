#pragma once
#include "Components.h"

namespace Okay
{
	struct CompPointLight
	{
		static const Components ID = Components::PointLight;

		CompPointLight() 
		:intensity(2.f), colour(WHITE), attenuation(0.2f, 0.0f) { }

		CompPointLight(float intensity, Float3 colour, Float2 attentuation)
			:intensity(intensity), colour(colour), attenuation(attentuation) { }

		float intensity;
		Float3 colour;
		Float2 attenuation;
	};
}