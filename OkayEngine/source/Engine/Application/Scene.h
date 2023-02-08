#pragma once

#include "Entt/entt.hpp"

namespace Okay
{
	class Entity;
	class Renderer;

	class Scene
	{
	public:
		Scene(Renderer& renderer);
		~Scene();

		Entity createEntity();
		void destroyEntity(const Entity& entity);
		void destroyEntity(const entt::entity& entity);

		void setMainCamera(const Entity& entity);
		Entity getMainCamera();
		inline entt::registry& getRegistry();

		void start();
		void update();
		void submit();
		void end();

	private:
		entt::entity mainCamera;
		Renderer& renderer;
		entt::registry registry;
	};

	inline entt::registry& Scene::getRegistry()	 { return registry; }
}