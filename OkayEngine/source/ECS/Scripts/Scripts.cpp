#include "Scripts.h"
#include "Engine/Engine.h"

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