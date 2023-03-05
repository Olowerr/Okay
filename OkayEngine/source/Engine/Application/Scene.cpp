#include "Scene.h"
#include "Entity.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/MeshComponent.h"
#include "Engine/Graphics/Renderer.h"

namespace Okay
{
    Scene::Scene()
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

    void Scene::updateCamerasAspectRatio(uint32_t width, uint32_t height)
    {
        registry.view<Camera>().each([=](Camera& camera)
        {
            camera.onTargetResize((float)width, (float)height);
        });
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
    
    void Scene::submit(Renderer* pRenderer)
    {
        // TODO: Should the renderer get a Scene* or the scene a Renderer* :thonk:
        // Submiting this way might be good for lights, since the data needs to be copied to the GPU anyway

        auto transformView = registry.view<Transform>(); // TODO: SExclude static entities
        for (entt::entity entity : transformView)
            transformView.get<Transform>(entity).calculateMatrix();

        auto meshView = registry.view<MeshComponent, Transform>();
        for (entt::entity entity : meshView)
            pRenderer->submit(meshView.get<MeshComponent>(entity), meshView.get<Transform>(entity));
    
#if 0 // Animations
        auto skeletalView = registry.view<Okay::CompSkeletalMesh, Transform>();
        for (entt::entity entity : skeletalView)
            ren.SumbitSkeletal(&skeletalView.get<Okay::CompSkeletalMesh>(entity), &skeletalView.get<Transform>(entity));
#endif
    
        auto pointLightView = registry.view<PointLight, Transform>();
        for (entt::entity entity : pointLightView)
            pRenderer->submit(pointLightView.get<PointLight>(entity), pointLightView.get<Transform>(entity));
        
        auto dirLightView = registry.view<DirectionalLight, Transform>();
        for (entt::entity entity : dirLightView)
            pRenderer->submit(dirLightView.get<DirectionalLight>(entity), dirLightView.get<Transform>(entity));

    }
    
    void Scene::end()
    {
        registry.view<ScriptComponent>().each([](ScriptComponent& script)
        {
            script.destroy();
        });
    }
}
