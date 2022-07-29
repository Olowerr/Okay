#pragma once

#include "Entt/entt.hpp"

class Entity;
class Scene
{
public:
	Scene();
	~Scene();

	Entity CreateEntity();
	void DestroyEntity(Entity entity);

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

/*

	Move All ImGui stuff into an Editor
	could just be a class, maybe different project ?

*/