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
    entity.AddComponent<Okay::CompTag>("Entity " + std::to_string((size_t)entity.GetID()));

    return entity;
}

void Scene::DestroyEntity(Entity entity)
{
    registry.destroy(entity);
}

void Scene::Start()
{
    //Entity entity = CreateEntity();
    //entity.AddComponent<Okay::CompPointLight>();
}

void Scene::Update()
{
    Renderer& ren = Okay::Engine::GetRenderer();

    const auto& meshView = registry.view<Okay::CompMesh, Okay::CompTransform>();
    for (auto& entity : meshView)
        ren.Submit(&meshView.get<Okay::CompMesh>(entity), &meshView.get<Okay::CompTransform>(entity));


    const auto& lightView = registry.view<Okay::CompPointLight, Okay::CompTransform>();
    for (auto& entity : lightView)
        ren.SubmitLight(&lightView.get<Okay::CompPointLight>(entity), &lightView.get<Okay::CompTransform>(entity));
}

void Scene::Stop()
{
    //const auto& view = registry.view<Okay::CompPointLight>();
    //for (auto& entity : view)
    //    registry.destroy(entity);
}
