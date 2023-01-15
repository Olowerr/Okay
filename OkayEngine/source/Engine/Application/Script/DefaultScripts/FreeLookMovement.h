#pragma once
#include "../ScriptBehaviour.h"

namespace Okay 
{
	class FreeLookMovement : public ScriptBehaviour
	{
	public:
		FreeLookMovement(Entity entity, float baseSpeed = 10.f);
		~FreeLookMovement();

		void start() override;
		void update() override;
		void destroy() override;

		float getSpeed() const;
		void setSpeed(float speed);

	private:
		float baseSpeed;

	};
}