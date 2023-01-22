#include "LerpList.h"

#include "imgui/imgui.h"

namespace Okay
{
	LerpList::LerpList()
		:guiPoint(0.f), guiSelIdx(Okay::INVALID_UINT)
	{
	}

	LerpList::LerpList(size_t numPoints)
		:guiPoint(0.f), guiSelIdx(Okay::INVALID_UINT)
	{
		points.reserve(numPoints);
	}

	LerpList::~LerpList()
	{
	}

	void LerpList::addPoint(float position, float value)
	{
		// TODO: Change to insert-sort

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

	bool LerpList::imgui(const char* label)
	{
		if (!ImGui::Begin(label))
		{
			ImGui::End();
			return false;
		}

		bool pressed = false;

		ImGui::PushItemWidth(-15.f);

		ImGui::Text("Values");
		ImGui::SameLine();
		ImGui::DragFloat2("##NewPoint", &guiPoint.x, 0.001f, 0.0f, 0.0f, "%.7f");

		if (ImGui::Button("Add", ImVec2(50.f, 20.f)))
		{
			addPoint(guiPoint.x, guiPoint.y);
			guiPoint.x = guiPoint.y = 0.f;
			guiSelIdx = Okay::INVALID_UINT;
			pressed = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Remove", ImVec2(50.f, 20.f)))
		{
			removePoint(guiSelIdx);
			pressed = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Modify", ImVec2(50.f, 20.f)))
		{
			removePoint(guiSelIdx);
			addPoint(guiPoint.x, guiPoint.y);
			pressed = true;
		}

		ImGui::Separator();
		for (uint32_t i = 0; i < (uint32_t)points.size(); i++)
		{
			char textBuffer[64]{};
			sprintf_s(textBuffer, "Point %u: %.7f | %.7f", i, points[i].x, points[i].y);
			if (ImGui::Selectable(textBuffer, i == guiSelIdx))
			{
				guiPoint = points[i];
				guiSelIdx = i;
			}

		}

		ImGui::PopItemWidth();
		ImGui::End();

		return pressed;
	}

}
