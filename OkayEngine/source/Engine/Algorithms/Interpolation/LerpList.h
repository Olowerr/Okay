#pragma once

#include "Engine/Okay/Okay.h"

#include <vector>

namespace Okay
{
	/*
		LerpList helps with linear interpolation on a set of points.
		Each point has a corresponding position and value.
		All positions are between 0.0f - 1.0f.
		Values are independent and can be anything.
	*/
	class LerpList
	{
	public:
		LerpList();
		LerpList(size_t numPoints);
		~LerpList();
		
		void addPoint(float position, float value);
		float sample(float position);

	private:
		std::vector<glm::vec2> points;
	};
}