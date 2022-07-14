#pragma once

#include "Entt/entt.hpp"

class Entity;
class Scene
{
public:
	Scene();
	~Scene();

	Entity CreateEntity();
	entt::registry& GetRegistry() { return registry; }


	entt::registry registry;
private:
	
};