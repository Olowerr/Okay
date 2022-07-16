#include "Scene.h"
#include "Entity.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

Entity Scene::CreateEntity()
{
    Entity entity(registry.create(), this);
    entity.AddComponent<Okay::CompTransform>();

    return entity;
}

void Scene::Start()
{

}

void Scene::Update()
{
}
