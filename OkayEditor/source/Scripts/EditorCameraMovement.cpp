#include "EditorCameraMovement.h"

#include <Engine/Application/Input/Input.h>
#include <Engine/Application/Time.h>
#include <Engine/Components/Transform.h>
#include <Engine/Application/Window.h>

EditorCamera::EditorCamera(Okay::Entity entity)
	:ScriptBehaviour(entity), targetPos(0.f), targetDist(10.f), rotSkipTimer(0.f)
{
}

EditorCamera::~EditorCamera()
{
}

void EditorCamera::start()
{
	using namespace Okay;

	Transform& tra = getComponent<Transform>();

	tra.rotation.x = 0.5f;
	tra.calculateMatrix();

	tra.position = targetPos - tra.forward() * 10.f;
}

void EditorCamera::update()
{
	using namespace Okay;

	targetDist += Input::getMouseWheelDir() * -1.f * targetDist * 0.06f;
	if (targetDist < 1.f) 
		targetDist = 1.f;

	Transform& tra = getComponent<Transform>();
	glm::vec3 fwd = tra.forward();

	tra.position = targetPos - fwd * targetDist;

	// Only apply rotation if L_ALT and leftMouse is down
	if (!Input::isKeyDown(Key::L_ALT) || !Input::leftMouseDown())
		return;

	if (!skip)
	{
		const float xRot = Input::getMouseXDelta();
		const float yRot = Input::getMouseYDelta();

		tra.rotation.x += yRot * 0.003f;
		tra.rotation.y += xRot * 0.003f;

		tra.calculateMatrix();
		fwd = tra.forward();

		const glm::vec3 right = tra.right();
		tra.position = targetPos - fwd * targetDist;
	}
	else
	{
		rotSkipTimer -= Time::getApplicationDT();
		skip = rotSkipTimer > 0.f;
	}


	// Loop around mouse if moved outside window
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
