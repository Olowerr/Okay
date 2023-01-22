#pragma once

#include "Engine/Okay/Okay.h"

#include <vector>

namespace Okay
{
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

		bool imgui(const char* label);

	private:
		std::vector<glm::vec2> points;

		glm::vec2 guiPoint;
		uint32_t guiSelIdx;
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