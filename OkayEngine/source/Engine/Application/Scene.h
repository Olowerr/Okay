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
		void destroyEntity(Entity entity);

		inline entt::registry& getRegistry();

		void start();
		void update();
		void submit();
		void end();

	private:
		Renderer& renderer;
		entt::registry registry;
	};

	inline entt::registry& Scene::getRegistry()
	{
		return registry;
	}

}