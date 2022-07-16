#pragma once

#include "Scene.h"
#include "Entt/Entt.hpp"

class Entity
{
public:
	Entity() 
	 :entityId(entt::null), pScene() { }
	Entity(entt::entity id, Scene* scene)
		:entityId(id), pScene(scene) { }

	template<typename T, typename... Args>
	T& AddComponent(Args... args)
	{
		return pScene->GetRegistry().emplace<T>(entityId, std::forward<Args>(args)...);
	}

	template<typename T>
	bool HasComponent()
	{
		return false;
		//return pScene->GetRegistry().has<T>(entityId); // has no exists
	}

	template<typename T>
	T& GetComponent()
	{
		return pScene->registry.get<T>(entityId);
	}



private:
	entt::entity entityId;
	Scene* pScene; // mmmmm .-.
};