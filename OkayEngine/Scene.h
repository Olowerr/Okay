#pragma once

#include "Entt/entt.hpp"
#include "Renderer.h"

class Entity;
class Scene
{
public:
	Scene(Renderer& renderer);
	~Scene();

	Entity CreateEntity();
	entt::registry& GetRegistry() { return registry; }

	// Maybe temp
	void Start();
	void Update();

	entt::registry registry;
private:
	Renderer& renderer;
};