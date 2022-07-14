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

	template<typename T>
	void AddComponent()
	{
		pScene->GetRegistry().emplace<T>(entityId);
	}

	template<typename T>
	bool HasComponent()
	{
		return pScene->GetRegistry().has<T>(entityId);
	}

	template<typename T>
	void GetComponent()
	{
		return pScene->GetRegistry().get<T>(entityId);
	}



private:
	entt::entity entityId;
	Scene* pScene; // mmmmm .-.
};