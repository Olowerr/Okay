#pragma once
#include "Engine/Application/Entity.h"

namespace Okay
{
	class ScriptBehaviour
	{
	public:
		ScriptBehaviour(Entity entity)
			:entity(entity) { }
	
		virtual ~ScriptBehaviour() { }
	
		virtual void start()	{ }
		virtual void update()	{ }
		virtual void destroy()	{ }
	
	protected:
		template<typename T, typename... Args>
		inline T& addComponent(Args&&... args);
		
		template<typename T>
		inline bool hasComponent();

		template<typename T>
		inline T& getComponent();

		template<typename T>
		inline bool removeComponent();

		template<typename T>
		inline T& getScript();

		inline Entity getEntity();
	
	private:
		Entity entity;
	};

	inline Entity ScriptBehaviour::getEntity() { return entity; }

	template<typename T, typename... Args>
	inline T& ScriptBehaviour::addComponent(Args&&... args)
	{
		return entity.addComponent<T>(args...);
	}

	template<typename T>
	inline bool ScriptBehaviour::hasComponent()
	{
		return entity.hasComponent<T>();
	}

	template<typename T>
	inline T& ScriptBehaviour::getComponent()
	{
		return entity.getComponent<T>();
	}

	template<typename T>
	inline bool ScriptBehaviour::removeComponent()
	{
		return entity.removeComponent<T>();
	}

	template<typename T>
	inline T& ScriptBehaviour::getScript()
	{
		return entity.getScript<T>();
	}
}
