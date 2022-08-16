#pragma once
#include "Entity.h"
#include "Okay.h"
#include "Components.h"

class ScriptBehaviour
{
public:
	ScriptBehaviour(Entity entity)
		:entity(entity) { }

	virtual ~ScriptBehaviour() { }

	virtual void Start() { }
	virtual void Update() { }
	virtual void Destroy() { }

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

private:
	Entity entity;

};

class CompScript
{
public:

	CompScript() = default;
	~CompScript() { OkayDelete(entity); }

	CompScript(ScriptBehaviour* pScript)
		:entity(pScript) { }

	void Start() { entity->Start();  }
	void Update() { entity->Update(); }
	void Destroy()  { entity->Destroy(); }

private:
	ScriptBehaviour* entity;
};





class RotateScript : public ScriptBehaviour
{
public:
	RotateScript(Entity entity)
		:ScriptBehaviour(entity) { }

	void Update() override;

private:
};


class HoverScript : public ScriptBehaviour
{
public:
	HoverScript(Entity entity)
		:ScriptBehaviour(entity), initPosY(0.f) { }

	void Start() override { initPosY = GetComponent<Okay::CompTransform>().position.y; }
	void Update() override;
private:
	float initPosY;
};