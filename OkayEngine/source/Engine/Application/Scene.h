#pragma once

#include "Entt/entt.hpp"

class Entity;
class Scene
{
public:
	Scene();
	~Scene();

	Entity createEntity();
	void destroyEntity(Entity entity);

	entt::registry& getRegistry() { return registry; }


	void start();
	void update();
	void submit();
	void end();


private:
	
	entt::registry registry;
};