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

	template<typename T>
	T& GetScript()
	{
		return entity.GetScript<T>();
	}

private:
	Entity entity;
};

class CompScript
{
public:

	CompScript() = default;
	~CompScript() { for (UINT i = 0; i < numScripts; i++) OkayDelete(pScripts[i]); }

	void AddScript(ScriptBehaviour* pScript)
	{
		if (numScripts >= MaxScripts)
			return;

		pScripts[numScripts++] = pScript;
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

	void Start()	{ for (UINT i = 0; i < numScripts; i++) pScripts[i]->Start();	}
	void Update()	{ for (UINT i = 0; i < numScripts; i++) pScripts[i]->Update();	}
	void Destroy()	{ for (UINT i = 0; i < numScripts; i++) pScripts[i]->Destroy(); }

private:
	static const UINT MaxScripts = 5;

	UINT numScripts = 0;
	ScriptBehaviour* pScripts[MaxScripts];
};

class ScriptCameraMovement : public ScriptBehaviour
{
public:
	ScriptCameraMovement(Entity entity)
		:ScriptBehaviour(entity)
		, tra(entity.GetComponent<Okay::CompTransform>())
		, cam(entity.GetComponent<Okay::CompCamera>())
		, pos(), fwd(), up()
		, camRot()
	{ };

	void Start();
	void Update() override;

	//private:
	Okay::CompTransform& tra;
	Okay::CompCamera& cam;

	float sens = 2.f;
	Okay::Float2 camRot;

	DirectX::XMVECTOR pos;
	DirectX::XMVECTOR fwd, up;
};

class ScriptBasicMovement : public ScriptBehaviour
{
public:
	ScriptBasicMovement(Entity entity)
		:ScriptBehaviour(entity)
		, tra(entity.GetComponent<Okay::CompTransform>())
		, cam(nullptr)
	{ }

	void Start() override;
	void Update() override;


private:
	Okay::CompTransform& tra;
	ScriptCameraMovement* cam;
};


class ScriptRotate : public ScriptBehaviour
{
public:
	ScriptRotate(Entity entity)
		:ScriptBehaviour(entity) { }

	void Update() override;

private:
};

class ScriptHover: public ScriptBehaviour
{
public:
	ScriptHover(Entity entity)
		:ScriptBehaviour(entity), initPosY(0.f) { }

	void Start() override { initPosY = GetComponent<Okay::CompTransform>().position.y; }
	void Update() override;
private:
	float initPosY;
};