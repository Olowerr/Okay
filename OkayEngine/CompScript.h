#pragma once
//#include "Scripts.h"
#include "Okay.h"

class ScriptBehaviour;
class Entity;

class CompScript
{
public:

	CompScript() = default;
	~CompScript();

	template<typename T>
	T& AddScript(Entity entity)
	{
		if (numScripts >= MaxScripts)
			assert(false);

		for (UINT i = 0; i < numScripts; i++)
		{
			if (dynamic_cast<T*>(pScripts[i]))
				return *(T*)pScripts[i];
		}

		//pScripts[numScripts] = new T(entity);
		//return *(T*)pScripts[numScripts++];

		return *(T*)(pScripts[numScripts++] = new T(entity));
	}

	template<typename T>
	T& GetScript()
	{
		for (UINT i = 0; i < numScripts; i++)
		{
			if (dynamic_cast<T*>(pScripts[i]))
				return *(T*)pScripts[i];
		}

		assert(false);
	}

	void Start();
	void Update();
	void Destroy();

private:
	static const UINT MaxScripts = 10;

	UINT numScripts = 0;
	ScriptBehaviour* pScripts[MaxScripts]{};
};