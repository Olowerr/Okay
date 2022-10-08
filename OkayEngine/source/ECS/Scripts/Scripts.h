#pragma once
#include "ECS/ScriptBehaviour.h"
#include "ECS/Components/Components.h"

class ScriptRotate : public ScriptBehaviour
{
public:
	ScriptRotate(Entity entity)
		:ScriptBehaviour(entity) { }

	void Update() override;
};

class ScriptHover : public ScriptBehaviour
{
public:
	ScriptHover(Entity entity)
		:ScriptBehaviour(entity), initPosY(0.f) { }

	void Start() override { initPosY = GetComponent<Okay::CompTransform>().position.y; }
	void Update() override;
private:
	float initPosY;
};