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


	void Start();
	void Update();
	void Submit();
	void End();


private:
	
	entt::registry registry;
};