#include "Scene.h"
#include "Engine.h"
#include <algorithm>

// temp
#include "CompLights.h"

Scene::Scene()
    :index(-1)
{
}
    

Scene::~Scene()
{
}

Entity Scene::CreateEntity()
{
    Entity entity(registry.create(), this);
    entity.AddComponent<Okay::CompTransform>();
    entity.AddComponent<Okay::CompTag>("Entity " + std::to_string((size_t)registry.alive()));

    return entity;
}

void Scene::DestroyEntity(Entity entity)
{
    registry.destroy(entity);
}

void Scene::Start()
{
    Entity entity = CreateEntity();
    entity.AddComponent<Okay::CompPointLight>();
}

void Scene::Update()
{
    const auto& group = registry.group<Okay::CompMesh, Okay::CompTransform>();

    Renderer& ren = Okay::Engine::GetRenderer();
    for (auto& entity : group)
        ren.Submit(&group.get<Okay::CompMesh>(entity), &group.get<Okay::CompTransform>(entity));

    const auto& view = registry.view<Okay::CompPointLight>();
    for (auto& entity : view)
        ren.Submit(&view.get<Okay::CompPointLight>(entity), &view.get<Okay::CompTransform>(entity));
}

void Scene::Stop()
{
    const auto& view = registry.view<Okay::CompPointLight>();
    for (auto& entity : view)
        registry.destroy(entity);
}
