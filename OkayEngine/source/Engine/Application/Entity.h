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
	T& addComponent(Args&&... args)
	{
		return pScene->getRegistry().emplace<T>(entityId, std::forward<Args>(args)...);
	}

	template<typename... T>
	bool hasComponent()
	{
		return pScene->getRegistry().all_of<T...>(entityId);
	}

	template<typename T>
	T& getComponent()
	{
		return pScene->getRegistry().get<T>(entityId);
	}

	template<typename T>
	bool removeComponent()
	{
		return pScene->getRegistry().remove<T>(entityId);
	}


	operator entt::entity()	{ return entityId; }
	operator entt::entity() const { return entityId; }

	entt::entity getID() const	{ return entityId; }
	bool isValid() const		{ return entityId != entt::null; }

private:
	entt::entity entityId;
	Scene* pScene;
};