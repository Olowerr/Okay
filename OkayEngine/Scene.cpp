#include "Scene.h"
#include "Engine.h"

Scene::Scene(Renderer& renderer)
    :renderer(renderer)
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
    const auto& group = registry.group<Okay::CompMesh, Okay::CompTransform>();
    
    for (auto& entity : group)
        renderer.Submit(&group.get<Okay::CompMesh>(entity), &group.get<Okay::CompTransform>(entity));
    
}
