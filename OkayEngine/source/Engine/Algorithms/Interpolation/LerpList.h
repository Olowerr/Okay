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

		inline void removePoint(uint32_t index);
		inline void setValue(uint32_t index, float value);

		inline const std::vector<glm::vec2>& getPoints() const;


	private:
		std::vector<glm::vec2> points;
	};

	inline void LerpList::removePoint(uint32_t index)
	{
		if (index >= (uint32_t)points.size())
			return;

		points.erase(points.begin() + index);
	}

	inline void LerpList::setValue(uint32_t index, float value)
	{
		if (index >= (uint32_t)points.size())
			return;
		
		points[index].y = value;
	}

	inline const std::vector<glm::vec2>& LerpList::getPoints() const
	{
		return points;
	}
}