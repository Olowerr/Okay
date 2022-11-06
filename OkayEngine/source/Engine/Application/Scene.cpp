#include "Scene.h"
#include "Entity.h"
#include "Engine/Components/Transform.h"

Scene::Scene()
{
}
    

Scene::~Scene()
{
}

Entity Scene::CreateEntity()
{
    Entity entity(registry.create(), this);
    entity.AddComponent<Okay::Transform>();

    return entity;
}

void Scene::DestroyEntity(Entity entity)
{
    registry.destroy(entity);
}

void Scene::Start()
{
    
}

void Scene::Update()
{

}

void Scene::Submit()
{

}

void Scene::End()
{

}
