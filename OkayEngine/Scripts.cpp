#include "Scripts.h"
#include "Engine.h"

void RotateScript::Update()
{
	auto& tra = GetComponent<Okay::CompTransform>();
	tra.rotation.y += 2.f * Okay::Engine::GetDT();
	tra.CalcMatrix();
}

void HoverScript::Update()
{
	auto& tra = GetComponent<Okay::CompTransform>();
	tra.position.y = initPosY + 1.5f * std::sin(Okay::Engine::GetUpTime());
	tra.CalcMatrix();
}
