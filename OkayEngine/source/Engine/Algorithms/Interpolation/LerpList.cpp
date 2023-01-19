#include "LerpList.h"
#include <algorithm>

namespace Okay
{
	LerpList::LerpList()
	{
	}

	LerpList::LerpList(size_t numPoints)
	{
		points.reserve(numPoints);
	}

	LerpList::~LerpList()
	{
	}

	void LerpList::addPoint(float position, float value)
	{
		const glm::vec2 newPoint(position, value);

		if (!points.size())
		{
			points.emplace_back(newPoint);
			return;
		}

		if (position < points[0].x)
		{
			points.emplace(points.begin(), newPoint);
			return;
		}

		int size = (int)points.size();
		for (int i = 0; i < size - 1; i++)
		{
			if (position > points[i].x && position < points[(size_t)i + 1].x)
			{
				points.emplace(points.begin() + i + 1, newPoint);
				return;
			}
		}

		points.emplace_back(newPoint);
	}

	float LerpList::sample(float position)
	{
		for (size_t i = 0; i < points.size() - 1; i++)
		{
			if (position > points[i].x && position < points[i + 1].x)
			{
				return glm::mix(points[i].y, points[i + 1].y, (position - points[i].x) / (points[i + 1].x - points[i].x));
			}
		}

		return 0.f;
	}

}
