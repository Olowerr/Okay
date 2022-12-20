#pragma once

#include "Scene.h"

namespace Okay
{
	class Entity
	{
	public:
		Entity()
			:entityId(entt::null), pScene(nullptr) { }
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

		operator entt::entity() { return entityId; }
		operator entt::entity() const { return entityId; }

		entt::entity getID() const { return entityId; }
		bool isValid() const { return entityId != entt::null; }

	private:
		Scene* pScene;
		entt::entity entityId;
		// Due to padding, another 4 bytes can fit here for free
		// What to add thooo :thonk:
	};
}