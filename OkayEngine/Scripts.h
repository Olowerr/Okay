#pragma once
#include "ScriptBehaviour.h"
#include "Components.h"

#include <ctime>

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

	Okay::Float2 camRot;

	DirectX::XMVECTOR pos;
	DirectX::XMVECTOR fwd, up;


	static DirectX::XMVECTOR GetRandVec(float scalar)
	{
		return DirectX::XMVectorSet(
			((rand() % 200) * 0.01f - 1.f) * scalar,
			((rand() % 200) * 0.01f - 1.f) * scalar,
			((rand() % 200) * 0.01f - 1.f) * scalar, 
			1.f);
	}
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