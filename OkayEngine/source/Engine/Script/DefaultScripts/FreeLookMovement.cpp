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
		const float frameSpeed = Time::getDT() * baseSpeed * (Input::isKeyDown(Key::L_SHIFT) ? 10.f : 1.f);

		const float xInput = (float)Input::isKeyDown(Key::D) - (float)Input::isKeyDown(Key::A); 
		const float yInput = (float)Input::isKeyDown(Key::SPACE) - (float)Input::isKeyDown(Key::L_CTRL);
		const float zInput = (float)Input::isKeyDown(Key::W) - (float)Input::isKeyDown(Key::S);
		const float xRot = (float)Input::isKeyDown(Key::DOWN) - (float)Input::isKeyDown(Key::UP);
		const float yRot = (float)Input::isKeyDown(Key::RIGHT) - (float)Input::isKeyDown(Key::LEFT);

		Transform& tra = getComponent<Transform>();
		const glm::vec3 fwd = tra.forward();
		const glm::vec3 right = tra.right();

		tra.position += (right * xInput + fwd * zInput) * frameSpeed;
		tra.position.y += yInput * frameSpeed;

		tra.rotation.x += xRot * 3.f * Time::getDT();
		tra.rotation.y += yRot * 3.f * Time::getDT();
	}

	void FreeLookMovement::destroy()
	{
	}
}
