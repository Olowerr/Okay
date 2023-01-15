#include "ScriptComponent.h"
#include "ScriptBehaviour.h"

namespace Okay
{
	ScriptComponent::ScriptComponent()
		:numScripts(0u), pScripts{}
	{
	}
	
	ScriptComponent::~ScriptComponent()
	{
		for (uint32_t i = 0; i < numScripts; i++)
			OKAY_DELETE(pScripts[i]);
	}
	
	void ScriptComponent::start()	
	{
		for (uint32_t i = 0; i < numScripts; i++)
			pScripts[i]->start();
	}
	
	void ScriptComponent::update()	
	{
		for (uint32_t i = 0; i < numScripts; i++)
			pScripts[i]->update(); 
	}
	
	void ScriptComponent::destroy()  
	{
		for (uint32_t i = 0; i < numScripts; i++) 
			pScripts[i]->destroy();
	}
}