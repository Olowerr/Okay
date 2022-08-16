#include "Scene.h"
#include "Engine.h"
#include "Scripts.h"

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

    // Temp
    last = entity.GetID();

    return entity;
}

void Scene::DestroyEntity(Entity entity)
{
    registry.destroy(entity);
}

void Scene::Start()
{
    // Temp
    Entity test(last, this);
   // test.AddScript<RotateScript>();
    test.AddScript<HoverScript>();

    registry.view<CompScript>().each([](CompScript& script)
    {
        script.Start();
    });
}

void Scene::Update()
{
    registry.view<CompScript>().each([](CompScript& script)
    {
        script.Update();
    });
}

void Scene::Submit()
{
    Renderer& ren = Okay::Engine::GetRenderer();

    const auto& meshView = registry.view<Okay::CompMesh, Okay::CompTransform>();
    for (auto& entity : meshView)
        ren.Submit(&meshView.get<Okay::CompMesh>(entity), &meshView.get<Okay::CompTransform>(entity));


    const auto& lightView = registry.view<Okay::CompPointLight, Okay::CompTransform>();
    for (auto& entity : lightView)
        ren.SubmitLight(&lightView.get<Okay::CompPointLight>(entity), &lightView.get<Okay::CompTransform>(entity));

}

void Scene::End()
{
    registry.view<CompScript>().each([](CompScript& script)
    {
        script.Destroy();
    });
}
