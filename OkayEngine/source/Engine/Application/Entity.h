#pragma once

#include "Scene.h"
#include "Engine/Okay/Okay.h"
#include "Script/ScriptComponent.h"

// TODO: Change Scene* pScene to entt::registry* pReg
// So that Scene.h can include Entity.h and keep an (Okay::Entity) of the main camera instead

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

		// TODO: Move definitions outside of class declartion
		// and inline those that make sense

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			OKAY_ASSERT(pScene, "pScene was nullptr");
			// Assert if has or just replace? or just return it ? // don't just return it
			return pScene->getRegistry().emplace<T>(entityId, std::forward<Args>(args)...);
		}

		template<typename... T>
		bool hasComponent() const
		{
			OKAY_ASSERT(pScene, "pScene was nullptr");
			return pScene->getRegistry().all_of<T...>(entityId);
		}

		template<typename T>
		T& getComponent()
		{
			OKAY_ASSERT(pScene, "pScene was nullptr");
			OKAY_ASSERT(hasComponent<T>(), "Entity doesn't have the given component");
			return pScene->getRegistry().get<T>(entityId);
		}

		template<typename T>
		const T& getComponent() const
		{
			OKAY_ASSERT(pScene, "pScene was nullptr");
			OKAY_ASSERT(hasComponent<T>(), "Entity doesn't have the given component");
			return pScene->getRegistry().get<T>(entityId);
		}

		template<typename T>
		bool removeComponent()
		{
			OKAY_ASSERT(pScene, "pScene was nullptr");
			return pScene->getRegistry().remove<T>(entityId);
		}

		template<typename T, typename... Args>
		T& addScript(Args&&... args)
		{
			if (!hasComponent<ScriptComponent>())
				addComponent<ScriptComponent>();

			return getComponent<ScriptComponent>().addScript<T>(*this, args...);
		}

		template<typename T>
		T& getScript()
		{
			return getComponent<ScriptComponent>().getScript<T>();
		}


		operator entt::entity()		  { return entityId; }
		operator entt::entity() const { return entityId; }

		entt::entity getID() const { return entityId; }
		bool isValid() const 
		{
			if (!pScene)
				return false;

			return pScene->getRegistry().valid(entityId);
		}

	private:
		Scene* pScene;
		entt::entity entityId;
		// Due to padding, another 4 bytes can fit here for free
		// What to add thooo :thonk:
	};
}