#pragma once
#include "Engine/Okay/Okay.h"

namespace Okay
{
	class Entity;
	class ScriptBehaviour;

	class ScriptComponent
	{
	public:
		static const uint32_t MaxScripts = 10;

		ScriptComponent();
		~ScriptComponent();

		template<typename T, typename... Args>
		inline T& addScript(Entity entity, Args&&... args);
		
		template<typename T>
		inline T& getScript();

		template<typename T>
		inline void removeScript();

		void start();
		void update();
		void destroy();

	private:

		uint32_t numScripts;
		ScriptBehaviour* pScripts[MaxScripts];
	};
	
	template<typename T, typename... Args>
	inline T& ScriptComponent::addScript(Entity entity, Args&&... args)
	{
		OKAY_ASSERT(numScripts < MaxScripts, "Max number of scripts reached");
		return *(T*)(pScripts[numScripts++] = new T(entity, args...));
	}

	template<typename T>
	inline T& ScriptComponent::getScript()
	{
		for (uint32_t i = 0; i < numScripts; i++)
		{
			if (dynamic_cast<T*>(pScripts[i])) 
				return *(T*)pScripts[i];
		}

		OKAY_ASSERT(false, "The entity doesn't have the given script");
	}

	template<typename T>
	inline void ScriptComponent::removeScript()
	{
		for (uint32_t i = 0; i < numScripts; i++)
		{
			if (dynamic_cast<T*>(pScripts[i]))
			{
				OKAY_DELETE(pScripts[i]);
				std::swap(pScripts[i], pScripts[--numScripts]);
				return;
			}
		}
	}
}
