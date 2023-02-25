#pragma once

#include "Engine/Okay/Okay.h"
#include "Engine/Script/ScriptComponent.h"

#include "Entt/entt.hpp"

namespace Okay
{
	class Entity
	{
	public:
		Entity()
			:entityId(entt::null), pReg(nullptr) 
		{ }

		Entity(entt::entity id, entt::registry* pReg)
			:entityId((entt::entity)id), pReg(pReg) 
		{ }
		

		template<typename T, typename... Args>
		inline T& addComponent(Args&&... args);

		template<typename... T>
		inline bool hasComponent() const;

		template<typename T>
		inline T& getComponent();

		template<typename T>
		inline const T& getComponent() const;

		template<typename T>
		inline bool removeComponent();

		template<typename T, typename... Args>
		inline T& addScript(Args&&... args);

		template<typename T>
		inline T& getScript();

		template<typename T>
		inline void removeScript();

		inline operator entt::entity() const	{ return entityId; }
		inline uint32_t getID() const		{ return (uint32_t)entityId; }

		inline bool isValid() const				{ return pReg ? pReg->valid(entityId) : false; }

		inline bool operator== (const Okay::Entity& other) { return entityId == other.entityId; }

	private:
		entt::registry* pReg;
		entt::entity entityId;

		// Due to padding, another 4 bytes can fit here for free
		// What to add thooo :thonk:
	};


	template<typename T, typename... Args>
	inline T& Entity::addComponent(Args&&... args)
	{
		OKAY_ASSERT(pReg, "pReg was nullptr");
		return pReg->emplace<T>(entityId, std::forward<Args>(args)...);
	}

	template<typename... T>
	inline bool Entity::hasComponent() const
	{
		OKAY_ASSERT(pReg, "pReg was nullptr");
		return pReg->all_of<T...>(entityId);
	}

	template<typename T>
	inline T& Entity::getComponent()
	{
		OKAY_ASSERT(pReg, "pReg was nullptr");
		OKAY_ASSERT(hasComponent<T>(), "The entity doesn't have the given component");
		return pReg->get<T>(entityId);
	}

	template<typename T>
	inline const T& Entity::getComponent() const
	{
		OKAY_ASSERT(pReg, "pReg was nullptr");
		OKAY_ASSERT(hasComponent<T>(), "The entity doesn't have the given component");
		return pReg->get<T>(entityId);
	}

	template<typename T>
	inline bool Entity::removeComponent()
	{
		OKAY_ASSERT(pReg, "pReg was nullptr");
		return pReg->remove<T>(entityId);
	}

	template<typename T, typename... Args>
	inline T& Entity::addScript(Args&&... args)
	{
		if (!hasComponent<ScriptComponent>())
			addComponent<ScriptComponent>();

		return getComponent<ScriptComponent>().addScript<T>(*this, args...);
	}

	template<typename T>
	inline T& Entity::getScript()
	{
		OKAY_ASSERT(hasComponent<ScriptComponent>(), "The entity does not have any scripts");
		return getComponent<ScriptComponent>().getScript<T>();
	}

	template<typename T>
	inline void Entity::removeScript()
	{
		OKAY_ASSERT(hasComponent<ScriptComponent>(), "The entity does not have any scripts");
		getComponent<ScriptComponent>().removeScript<T>();
	} 
}