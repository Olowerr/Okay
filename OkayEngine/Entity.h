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
	T& AddComponent(Args&&... args)
	{
#ifdef EDITOR
		comps.emplace_back(&pScene->GetRegistry().emplace<T>(entityId, std::forward<Args>(args)...));
		return *(T*)comps.back();
#else
		return pScene->GetRegistry().emplace<T>(entityId, std::forward<Args>(args)...);
#endif // EDTIOR

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

	operator entt::entity() { return entityId; }
	operator entt::entity() const { return entityId; }

	entt::entity GetID() const { return entityId; }

private:
	entt::entity entityId;

	// Could FwdDeclare Entity in engine and call the engine functions from here
	// But then an entities wouldn't care which scene they're in
	// pScene makes it possible to open multiple scenes with their own entities
	Scene* pScene;

#ifdef EDITOR
	std::vector<Okay::BaseComp*> comps;
#endif // EDITOR

};