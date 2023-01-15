#include "FreeLookMovement.h"
#include "Engine/Components/Transform.h"
#include "Engine/Application/Input/Input.h"
#include "Engine/Application/Time.h"

namespace Okay
{
	FreeLookMovement::FreeLookMovement(Entity entity, float baseSpeed)
		:ScriptBehaviour(entity), baseSpeed(baseSpeed)
	{
	}
	
	FreeLookMovement::~FreeLookMovement()
	{
	}

	float FreeLookMovement::getSpeed() const
	{
		return baseSpeed;
	}

	void FreeLookMovement::setSpeed(float speed)
	{
		baseSpeed = speed;
	}

	void FreeLookMovement::start()
	{
	}

	void FreeLookMovement::update()
	{
		const float frameSpeed = baseSpeed * Time::getDT();
		const float xInput = (float)Input::isKeyDown(Keys::D) - (float)Input::isKeyDown(Keys::A);
		const float yInput = (float)Input::isKeyDown(Keys::SPACE) - (float)Input::isKeyDown(Keys::E);
		const float zInput = (float)Input::isKeyDown(Keys::W) - (float)Input::isKeyDown(Keys::S);
		//const float yRot = (float)Input::isKeyDown(Keys::LEFT)

		Transform& tra = getComponent<Transform>();
		const glm::vec3 fwd = tra.forward();
		const glm::vec3 right = tra.right();

		tra.position += (right * xInput + fwd * zInput) * frameSpeed;
		tra.position.y += yInput * frameSpeed;
	}

	void FreeLookMovement::destroy()
	{
	}
}
