#include "Scene.h"
#include "Entity.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/MeshComponent.h"
#include "Engine/Graphics/Renderer.h"

namespace Okay
{

    Scene::Scene(Renderer& renderer)
        :renderer(renderer)
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
    
    void Scene::destroyEntity(Entity&& entity)
    {
        registry.destroy(entity.getID());
    }
    
    void Scene::start()
    {
        
    }
    
    void Scene::update()
    {
    
    }
    
    void Scene::submit()
    {
        const auto& meshView = registry.view<MeshComponent, Transform>();
        for (auto& entity : meshView)
            renderer.submit(&meshView.get<MeshComponent>(entity), &meshView.get<Transform>(entity));
    
#if 0 // Animations
        const auto& skeletalView = registry.view<Okay::CompSkeletalMesh, Transform>();
        for (auto& entity : skeletalView)
            ren.SumbitSkeletal(&skeletalView.get<Okay::CompSkeletalMesh>(entity), &skeletalView.get<Transform>(entity));
#endif
    
#if 0 // Point lights
        const auto& lightView = registry.view<Okay::CompPointLight, Transform>();
        for (auto& entity : lightView)
            ren.SubmitLight(&lightView.get<Okay::CompPointLight>(entity), &lightView.get<Transform>(entity));
#endif
    }
    
    void Scene::end()
    {
    
    }
}
