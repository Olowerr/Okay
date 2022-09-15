#pragma once
#include "ScriptBehaviour.h"
#include "Components.h"

class ScriptCameraMovement : public ScriptBehaviour
{
public:
	ScriptCameraMovement(Entity entity)
		:ScriptBehaviour(entity)
		, tra(entity.GetComponent<Okay::CompTransform>())
		, cam(entity.GetComponent<Okay::CompCamera>())
		, pos(), fwd(), up()
		, camRot()
	{ };

	void Start();
	void Update() override;

	//private:
	Okay::CompTransform& tra;
	Okay::CompCamera& cam;

	float sens = 2.f;
	Okay::Float2 camRot;

	DirectX::XMVECTOR pos;
	DirectX::XMVECTOR fwd, up;
};

class ScriptBasicMovement : public ScriptBehaviour
{
public:
	ScriptBasicMovement(Entity entity)
		:ScriptBehaviour(entity)
		, tra(entity.GetComponent<Okay::CompTransform>())
		, cam(nullptr)
	{ }

	void Start() override;
	void Update() override;


private:
	Okay::CompTransform& tra;
	ScriptCameraMovement* cam;
};


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