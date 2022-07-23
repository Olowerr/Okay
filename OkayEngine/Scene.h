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
	std::vector<Entity>& GetEntities() { return entities; }

	// Maybe temp
	void Start();
	void Update();

private:
	entt::registry registry;

	// Maybe temp
	std::vector<Entity> entities;

	// Temp
	int index;
};