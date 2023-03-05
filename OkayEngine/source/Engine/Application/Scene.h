#pragma once

#include "Entt/entt.hpp"
#include "Entity.h"

#include "Engine/Graphics/Renderer.h"

namespace Okay
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity createEntity();
		inline void destroyEntity(const Entity& entity);
		inline void destroyEntity(entt::entity entity);

		inline void setMainCamera(const Entity& entity);
		inline void setSkyLight(const Entity& entity);
		inline void setSun(const Entity& entity);

		inline Entity getMainCamera();
		inline Entity getSkyLight();
		inline Entity getSun();

		inline entt::registry& getRegistry();

		void updateCamerasAspectRatio(uint32_t width, uint32_t height);

		void start();
		void update();
		void submit(Renderer* pRenderer);
		void end();

	private:
		entt::registry registry;

		Entity mainCamera;
		Entity skyLight;
		Entity sun;

	};

	inline void Scene::destroyEntity(const Entity& entity)	{ registry.destroy(entity); }
	inline void Scene::destroyEntity(entt::entity entity)	{ registry.destroy(entity); }

	inline void Scene::setMainCamera(const Entity& entity)	{ mainCamera = entity; }
	inline void Scene::setSkyLight(const Entity& entity)	{ skyLight = entity; }
	inline void Scene::setSun(const Entity& entity)			{ sun = entity; }

	inline Entity Scene::getMainCamera()					{ return mainCamera; }
	inline Entity Scene::getSkyLight()						{ return skyLight; }
	inline Entity Scene::getSun()							{ return sun; }

	inline entt::registry& Scene::getRegistry()				{ return registry; }
}