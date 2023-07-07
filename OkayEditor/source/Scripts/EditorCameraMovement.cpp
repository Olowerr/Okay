#include "EditorCameraMovement.h"

#include <Engine/Application/Input/Input.h>
#include <Engine/Application/Time.h>
#include <Engine/Components/Transform.h>
#include <Engine/Application/Window.h>
#include <Engine/Components/Camera.h>

#include "imgui/imgui.h"

EditorCamera::EditorCamera(Okay::Entity entity)
	:ScriptBehaviour(entity), targetPos(0.f), targetDist(10.f), rotSkipTimer(0.f)
{
	if (!entity.hasComponent<Okay::Camera>())
		entity.addComponent<Okay::Camera>();
}

EditorCamera::~EditorCamera()
{
}

void EditorCamera::start()
{// 2
	using namespace Okay;

	Transform& tra = getComponent<Transform>();

	tra.rotation.x = 0.5f;
	tra.calculateMatrix();

	tra.position = targetPos - tra.forward() * 10.f;
}

void EditorCamera::update()
{
	using namespace Okay;

	targetDist += std::min(Input::getMouseWheelDir() * -1.f * targetDist * 0.06f, 10.f);
	if (targetDist < 0.1f) 
		targetDist = 0.1f;

	Transform& tra = getComponent<Transform>();
	glm::vec3 fwd = tra.forward();
	tra.position = targetPos - fwd * targetDist;

	if (lerpToFocus)
	{
		const glm::vec3 entityPos = focusedEntity.getComponent<Transform>().position;
		targetPos = glm::mix(lerpStartPos, entityPos, focusLerpT);
		focusLerpT += Time::getApplicationDT() / FOCUS_TIME;

		if (focusLerpT >= 1.f)
		{
			targetPos = entityPos;
			lerpToFocus = false;
		}
	}

	if (!lerpToFocus && Input::isKeyPressed(Key::F) && focusedEntity.isValid())
	{
		lerpStartPos = targetPos;
		focusLerpT = 0.f;
		lerpToFocus = true;
	}


	if (!Input::leftMouseDown() || (!Input::isKeyDown(Key::L_ALT) && !Input::isKeyDown(L_SHIFT)))
		return;

	if (!skip)
	{
		const float xMouse = Input::getMouseXDelta();
		const float yMouse = Input::getMouseYDelta();

		// Rotation
		if (Input::isKeyDown(Key::L_ALT))
		{
			tra.rotation.x += yMouse * 0.003f;
			tra.rotation.y += xMouse * 0.003f;

			tra.calculateMatrix();
			fwd = tra.forward();

			tra.position = targetPos - fwd * targetDist;
		}

		// Movement
		else if (Input::isKeyDown(Key::L_SHIFT))
		{
			const glm::vec3 right = tra.right();
			const glm::vec3 up = tra.up();
			targetPos += ((right * -xMouse * 0.03f) + (up * yMouse * 0.03f)) * targetDist * 0.1f;
		}
	}
	else
	{
		rotSkipTimer -= Time::getApplicationDT();
		skip = rotSkipTimer > 0.f;
	}


	// Loop around mouse if is moved outside window
	Window* pWindow = Window::getActiveWindow();
	if (!pWindow)
		return;
	
	const glm::vec2 winSize(pWindow->getDimensions());
	if (Input::getMouseXPos() > winSize.x - PADDING)
	{
		Input::setMouseXPos(PADDING);
		rotSkipTimer = SKIP_DURATION;
		skip = true;
	}
	else if (Input::getMouseYPos() > winSize.y - PADDING)
	{
		Input::setMouseYPos(PADDING);
		rotSkipTimer = SKIP_DURATION;
		skip = true;
	}

	else if (Input::getMouseXPos() < PADDING)
	{
		Input::setMouseXPos(winSize.x - PADDING);
		rotSkipTimer = SKIP_DURATION;
		skip = true;
	}
	else if (Input::getMouseYPos() < PADDING)
	{
		Input::setMouseYPos(winSize.y - PADDING);
		rotSkipTimer = SKIP_DURATION;
		skip = true;
	}

}
