#include "Scripts.h"
#include "Engine.h"

void ScriptRotate::Update()
{
	auto& tra = GetComponent<Okay::CompTransform>();
	tra.rotation.y += 2.f * Okay::Engine::GetDT();
	tra.CalcMatrix();
}

void ScriptHover::Update()
{
	auto& tra = GetComponent<Okay::CompTransform>();
	tra.position.y = initPosY + 1.5f * std::sin(Okay::Engine::GetUpTime());
	tra.CalcMatrix();
}



void ScriptBasicMovement::Update()
{
	const float ZInput = Okay::Engine::GetKeyDown(Keys::W) ? 1.f : Okay::Engine::GetKeyDown(Keys::S) ? -1.f : 0.f;
	const float XInput = Okay::Engine::GetKeyDown(Keys::D) ? 1.f : Okay::Engine::GetKeyDown(Keys::A) ? -1.f : 0.f;
	const float Speed = 2.f * Okay::Engine::GetDT();

	tra.position.z += ZInput * Speed;
	tra.position.x += XInput * Speed;

	tra.CalcMatrix();
}

void ScriptCameraMovement::Update()
{
	using namespace DirectX;

	
	XMVECTOR vector = XMQuaternionRotationRollPitchYaw(0.f, tra.rotation.y, 0.f);
	fwd = XMVector3Rotate(Okay::FORWARD, vector);
	up = XMVector3Rotate(Okay::UP, vector);
	pos = XMVectorAdd(XMLoadFloat3(&tra.position), XMVectorScale(fwd, -5.f));

	printf("%f\n", Okay::Engine::GetMouseDeltaX());

	cam.Update(pos, XMLoadFloat3(&tra.position), up);
}
