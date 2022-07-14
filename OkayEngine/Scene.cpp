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
    return { registry.create(), this };
}
