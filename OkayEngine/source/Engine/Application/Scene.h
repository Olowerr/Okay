#pragma once

#include "Entt/entt.hpp"
#include "Entity.h"

namespace Okay
{
	class Renderer;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity createEntity();
		inline void destroyEntity(const Entity& entity);
		inline void destroyEntity(entt::entity entity);

		inline void setMainCamera(const Entity& entity);
		inline Entity getMainCamera();

		inline void setSkyLight(const Entity& entity);
		inline Entity getSkyLight();

		inline entt::registry& getRegistry();

		void updateCamerasAspectRatio(uint32_t width, uint32_t height);

		void start();
		void update();
		void submit(Renderer& renderer);
		void end();

	private:
		entt::registry registry;

		Entity mainCamera;
		Entity skyLight;
	};

	inline void Scene::destroyEntity(const Entity& entity)	{ registry.destroy(entity); }
	inline void Scene::destroyEntity(entt::entity entity)	{ registry.destroy(entity); }

	inline void Scene::setMainCamera(const Entity& entity)	{ if (entity.isValid()) mainCamera = entity; }
	inline Entity Scene::getMainCamera()					{ return mainCamera; }

	inline void Scene::setSkyLight(const Entity& entity)	{ if (entity.isValid()) skyLight = entity; }
	inline Entity Scene::getSkyLight()						{ return skyLight; }

	inline entt::registry& Scene::getRegistry()				{ return registry; }
}