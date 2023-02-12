#include "Scene.h"
#include "Entity.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/MeshComponent.h"
#include "Engine/Graphics/Renderer.h"

namespace Okay
{
    Scene::Scene(Renderer& renderer)
        :renderer(renderer), mainCamera(entt::null, &registry)
    {
    }
    
    Scene::~Scene()
    {
    }
    
    Entity Scene::createEntity()
    {
        Entity entity(registry.create(), &registry);
        entity.addComponent<Okay::Transform>();
        
        return entity;
    }

    void Scene::start()
    {
        registry.view<ScriptComponent>().each([](ScriptComponent& script)
        {
            script.start();
        });
    }
    
    void Scene::update()
    {
        registry.view<ScriptComponent>().each([](ScriptComponent& script)
        {
            script.update();
        });
    }
    
    void Scene::submit()
    {
        auto transformView = registry.view<Transform>(); // Exclude static entities
        for (entt::entity entity : transformView)
            transformView.get<Transform>(entity).calculateMatrix();

        auto meshView = registry.view<MeshComponent, Transform>();
        for (entt::entity entity : meshView)
            renderer.submit(meshView.get<MeshComponent>(entity), meshView.get<Transform>(entity));
    
#if 0 // Animations
        auto skeletalView = registry.view<Okay::CompSkeletalMesh, Transform>();
        for (entt::entity entity : skeletalView)
            ren.SumbitSkeletal(&skeletalView.get<Okay::CompSkeletalMesh>(entity), &skeletalView.get<Transform>(entity));
#endif
    
        auto pointLightView = registry.view<PointLight, Transform>();
        for (entt::entity entity : pointLightView)
            renderer.submit(pointLightView.get<PointLight>(entity), pointLightView.get<Transform>(entity));
        
        auto dirLightView = registry.view<DirectionalLight, Transform>();
        for (entt::entity entity : dirLightView)
            renderer.submit(dirLightView.get<DirectionalLight>(entity), dirLightView.get<Transform>(entity));

    }
    
    void Scene::end()
    {
        registry.view<ScriptComponent>().each([](ScriptComponent& script)
        {
            script.destroy();
        });
    }
}
