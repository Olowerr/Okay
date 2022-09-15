#pragma once
#include "Entity.h"

class ScriptBehaviour
{
public:
	ScriptBehaviour(Entity entity)
		:entity(entity) { }

	virtual ~ScriptBehaviour() { }

	virtual void Start()	{ }
	virtual void Update()	{ }
	virtual void Destroy()	{ }

protected:
	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		return entity.AddComponent<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	bool HasComponent()
	{
		return entity.HasComponent<T>();
	}

	template<typename T>
	T& GetComponent()
	{
		return entity.GetComponent<T>();
	}

	template<typename T>
	bool RemoveComponent()
	{
		return entity.RemoveComponent<T>();
	}

	template<typename T>
	T& GetScript()
	{
		return entity.GetScript<T>();
	}

private:
	Entity entity;
};
