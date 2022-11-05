#pragma once

#include "Scene.h"

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

	template<typename... T>
	bool HasComponent()
	{
		return pScene->GetRegistry().all_of<T...>(entityId);
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


	operator entt::entity()			{ return entityId; }
	operator entt::entity() const	{ return entityId; }

	entt::entity GetID() const	{ return entityId; }
	bool IsValid() const		{ return entityId != entt::null; }

private:
	entt::entity entityId;
	Scene* pScene;
};