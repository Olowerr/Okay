#include "CompScript.h"
#include "ScriptBehaviour.h"

CompScript::~CompScript()
{
	for (UINT i = 0; i < numScripts; i++)
		OkayDelete(pScripts[i]);
}

void CompScript::Start()	
{
	for (UINT i = 0; i < numScripts; i++)
		pScripts[i]->Start();
}

void CompScript::Update()	
{
	for (UINT i = 0; i < numScripts; i++)
		pScripts[i]->Update(); 
}

void CompScript::Destroy()  
{
	for (UINT i = 0; i < numScripts; i++) 
		pScripts[i]->Destroy();
}