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

		inline void setRenderer(Renderer* pRenderer);

		inline void setMainCamera(const Entity& entity);
		inline Entity getMainCamera();

		inline void setSkyLight(const Entity& entity);
		inline Entity getSkyLight();

		inline entt::registry& getRegistry();

		void updateCamerasAspectRatio(uint32_t width, uint32_t height);

		void start();
		void update();
		void submit();
		void end();

	private:
		Renderer* pRenderer;
		entt::registry registry;

		Entity mainCamera;
		Entity skyLight;
	};

	inline void Scene::setRenderer(Renderer* pRenderer)
	{
		OKAY_ASSERT(pRenderer, "pRenderer was nullptr");
		this->pRenderer = pRenderer;
	}

	inline void Scene::destroyEntity(const Entity& entity)	{ registry.destroy(entity); }
	inline void Scene::destroyEntity(entt::entity entity)	{ registry.destroy(entity); }

	inline Entity Scene::getMainCamera() { return mainCamera; }
	inline void Scene::setMainCamera(const Entity& entity)	
	{
		if (entity.isValid()) // Hmm
			mainCamera = entity; 

		OKAY_ASSERT(pRenderer, "pRenderer was nullptr");
		pRenderer->setCamera(entity);
	}

	inline Entity Scene::getSkyLight() { return skyLight; }
	inline void Scene::setSkyLight(const Entity& entity)	
	{
		if (entity.isValid()) 
			skyLight = entity; 

		OKAY_ASSERT(pRenderer, "pRenderer was nullptr");
		pRenderer->setSkyLight(skyLight);

	}

	inline entt::registry& Scene::getRegistry() { return registry; }
}