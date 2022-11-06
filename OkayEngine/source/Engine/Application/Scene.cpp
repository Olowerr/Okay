#include "Scene.h"
#include "Entity.h"
#include "Engine/Components/Transform.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

Entity Scene::createEntity()
{
    Entity entity(registry.create(), this);
    entity.addComponent<Okay::Transform>();
    
    return entity;
}

void Scene::destroyEntity(Entity entity)
{
    registry.destroy(entity);
}

void Scene::start()
{
    
}

void Scene::update()
{

}

void Scene::submit()
{

}

void Scene::end()
{

}
