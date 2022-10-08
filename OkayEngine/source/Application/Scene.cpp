#include "Scene.h"
#include "Engine/Engine.h"
#include "ECS/Scripts/Scripts.h"

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
    entity.AddComponent<Okay::CompTag>("Entity " + std::to_string((size_t)entity.GetID()));


    return entity;
}

void Scene::DestroyEntity(Entity entity)
{
    registry.destroy(entity);
}

void Scene::Start()
{
    Entity gobWalk = CreateEntity();
    gobWalk.AddComponent<Okay::CompSkeletalMesh>("gobWalk3").StartAnimation();

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

    registry.view<Okay::CompSkeletalMesh>().each([](Okay::CompSkeletalMesh& cMesh)
    {
        cMesh.UpdateAnimation();
    });


}

void Scene::Submit()
{
    Renderer& ren = Okay::Engine::GetRenderer();

    const auto& meshView = registry.view<Okay::CompMesh, Okay::CompTransform>();
    for (auto& entity : meshView)
        ren.Submit(&meshView.get<Okay::CompMesh>(entity), &meshView.get<Okay::CompTransform>(entity));


    const auto& skeletalView = registry.view<Okay::CompSkeletalMesh, Okay::CompTransform>();
    for (auto& entity : skeletalView)
        ren.SumbitSkeletal(&skeletalView.get<Okay::CompSkeletalMesh>(entity), &skeletalView.get<Okay::CompTransform>(entity));


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

    const auto& skeleltalView = registry.view<Okay::CompSkeletalMesh>();
    entt::registry& reg = registry;

    skeleltalView.each([&reg](entt::entity entity)
    {
        reg.destroy(entity);
    });
}
