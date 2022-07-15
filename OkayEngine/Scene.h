#pragma once

#include "Entt/entt.hpp"
#include "Components.h"

class Entity;
class Scene
{
public:
	Scene();
	~Scene();

	Entity CreateEntity();
	entt::registry& GetRegistry() { return registry; }

	// Maybe temp
	void Start();
	// Temp
	void Update();

	entt::registry registry;
private:
	
};