#pragma once

#include "Entt/entt.hpp"
#include "Entity.h"

namespace Okay
{
	class Renderer;

	class Scene
	{
	public:
		Scene(Renderer& renderer);
		~Scene();

		Entity createEntity();
		inline void destroyEntity(const Entity& entity);
		inline void destroyEntity(entt::entity entity);

		inline void setMainCamera(const Entity& entity);
		inline Entity getMainCamera();

		inline entt::registry& getRegistry();

		void start();
		void update();
		void submit();
		void end();

	private:
		entt::registry registry;
		Renderer& renderer;

		Entity mainCamera;
		Entity skyLight;
	};

	inline void Scene::destroyEntity(const Entity& entity)	{ registry.destroy(entity); }
	inline void Scene::destroyEntity(entt::entity entity)	{ registry.destroy(entity); }

	inline Entity Scene::getMainCamera()					{ return mainCamera; }
	inline void Scene::setMainCamera(const Entity& entity)	{ if (entity.isValid()) mainCamera = entity; }

	inline entt::registry& Scene::getRegistry()				{ return registry; }
}