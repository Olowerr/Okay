#pragma once

#include <Engine/Script/ScriptBehaviour.h>

#include <Engine/Components/Transform.h>
#include <Engine/Application/Input/Input.h>
#include <Engine/Application/Time.h>

#include "imgui/imgui.h"

class ThirdPersonMovement : public Okay::ScriptBehaviour
{
public:
	ThirdPersonMovement(Okay::Entity entity, Okay::Entity camera)
		:ScriptBehaviour(entity), camera(camera)
	{

	}
	~ThirdPersonMovement()
	{
	}

	void update() override
	{
		if (ImGui::Begin("3rd person movement"))
		{
			ImGui::PushItemWidth(-100.f);

			ImGui::DragFloat("Camera dist", &dist, 0.01f);
			ImGui::DragFloat("Cam Y offset", &yOffset, 0.01f);
			ImGui::DragFloat("Cam speed", &rotSpeed, 0.01f);
			ImGui::Separator();
			ImGui::DragFloat("Speed", &moveSpeed, 0.1f);

			ImGui::PopItemWidth();
		}
		ImGui::End();

		using namespace Okay;

		const float frameSpeed = Time::getDT() * (Input::isKeyDown(Keys::L_SHIFT) ? 10.f : Input::isKeyDown(Keys::L_CTRL) ? 0.1f : 1.f);
		const float xInput = (float)Input::isKeyDown(Keys::D) - (float)Input::isKeyDown(Keys::A);
		const float yInput = (float)Input::isKeyDown(Keys::SPACE) - (float)Input::isKeyDown(Keys::L_CTRL);
		const float zInput = (float)Input::isKeyDown(Keys::W) - (float)Input::isKeyDown(Keys::S);
		const float xRot = (float)Input::isKeyDown(Keys::DOWN) - (float)Input::isKeyDown(Keys::UP);
		const float yRot = (float)Input::isKeyDown(Keys::RIGHT) - (float)Input::isKeyDown(Keys::LEFT);

		Transform& charTra = getComponent<Transform>();
		Transform& camTra = camera.getComponent<Transform>();

		camTra.rotation.x += xRot * Time::getDT() * rotSpeed;
		camTra.rotation.y += yRot * Time::getDT() * rotSpeed;
		camTra.calculateMatrix();
		const glm::vec3 camFwd = camTra.forward();
		const glm::vec3 camRight = camTra.right();

		const glm::vec3 step = (camRight * xInput + camFwd * zInput) * frameSpeed * moveSpeed;
		charTra.position.x += step.x;
		charTra.position.z += step.z;

		camTra.position = (charTra.position + glm::vec3(0.f, yOffset, 0.f)) - camFwd * dist;
	}

private:
	Okay::Entity camera;
	float dist = 10.f;
	float yOffset = 5.f;
	float moveSpeed = 10.f;
	float rotSpeed = 3.f;

};