#pragma once

#include "Scene.h"
#include "CompScript.h"

class Entity
{
public:
	Entity() 
	 :entityId(entt::null), pScene() { }
	Entity(entt::entity id, Scene* scene)
		:entityId(id), pScene(scene) { }

	Entity& operator=(const Entity&) = default;

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		return pScene->GetRegistry().emplace<T>(entityId, std::forward<Args>(args)...);
	}

	template<typename T>
	bool HasComponent()
	{
		// apparently has() doesn't exist
		return pScene->GetRegistry().try_get<T>(entityId);
	}

	template<typename T>
	T& GetComponent()
	{
		return pScene->GetRegistry().get<T>(entityId);
	}

	template<typename T>
	bool RemoveComponent()
	{
		return pScene->GetRegistry().remove<T>(entityId);
	}

	template<typename T>
	T& AddScript()
	{
		if (!HasComponent<CompScript>())
			pScene->GetRegistry().emplace<CompScript>(entityId);

		//GetComponent<CompScript>().AddScript(new T(*this));
		return GetComponent<CompScript>().AddScript<T>(*this);
	}

	template<typename T>
	T& GetScript()
	{
		return GetComponent<CompScript>().GetScript<T>();
	}


	operator entt::entity()			{ return entityId; }
	operator entt::entity() const	{ return entityId; }

	entt::entity GetID() const	{ return entityId; }
	bool IsValid() const		{ return entityId != entt::null; }

	void SetInvalid() { entityId = entt::null; pScene = nullptr; }

private:
	entt::entity entityId;

	// Could FwdDeclare Entity in engine and call the engine functions from here
	// But then entities wouldn't care which scene they're in
	// pScene makes it possible to open multiple scenes with their own entities
	Scene* pScene;
};