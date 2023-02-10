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

	if (Input::isKeyDown(Key::F))
	{
		float x = Input::getDesktopMouseXPos();
		float y = Input::getDesktopMouseYPos();
		printf("%.0f, %.0f\n", x, y);
	}

	if (!Input::isKeyDown(Key::L_ALT) || !Input::leftMouseDown())
		return;

	//const float frameSpeed = Time::getDT() * speed;

	//const float xInput = (float)Input::isKeyDown(Key::D) - (float)Input::isKeyDown(Key::A);
	//const float yInput = (float)Input::isKeyDown(Key::SPACE) - (float)Input::isKeyDown(Key::L_CTRL);
	//const float zInput = (float)Input::isKeyDown(Key::W) - (float)Input::isKeyDown(Key::S);
	const float xRot = Input::getMouseXDelta();
	const float yRot = Input::getMouseYDelta();

	Transform& tra = getComponent<Transform>();
	
	if (!skip)
	{
		tra.rotation.x += yRot * 0.003f;
		tra.rotation.y += xRot * 0.003f;
	}
	else
	{
		rotSkipTimer -= Time::getApplicationDT();
		skip = rotSkipTimer > 0.f;
	}

	tra.calculateMatrix();
	const glm::vec3 fwd = tra.forward();
	const glm::vec3 right = tra.right();

	tra.position = targetPos - fwd * targetDist;

	// Move back mouse if moved outside window border
	Window* pWindow = Window::getActiveWindow();
	if (!pWindow)
		return;
	
	const glm::vec2 winSize(pWindow->getDimensions());
	if (Input::getMouseXPos() > winSize.x)
	{
		Input::setMouseXPos(PADDING);
		rotSkipTimer = SKIP_DURATION;
		skip = true;
	}
	else if (Input::getMouseYPos() > winSize.y)
	{
		Input::setMouseYPos(PADDING);
		rotSkipTimer = SKIP_DURATION;
		skip = true;
	}

	else if (Input::getMouseXPos() < 0.f)
	{
		Input::setMouseXPos(winSize.x - PADDING);
		rotSkipTimer = SKIP_DURATION;
		skip = true;
	}
	else if (Input::getMouseYPos() < 0.f)
	{
		Input::setMouseYPos(winSize.y - PADDING);
		rotSkipTimer = SKIP_DURATION;
		skip = true;
	}

}
