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
    
    void Scene::destroyEntity(const Entity& entity)
    {
        registry.destroy(entity);
    }

    void Scene::destroyEntity(const entt::entity& entity)
    {
        registry.destroy(entity);
    }
    
    Entity Okay::Scene::getMainCamera()
    {
        return Entity(mainCamera, this);
    }

    void Scene::setMainCamera(const Entity& entity) 
    {
        mainCamera = entity.getID(); 
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
        for (entt::entity entity : meshView)
            renderer.submit(&meshView.get<MeshComponent>(entity), &meshView.get<Transform>(entity));
    
#if 0 // Animations
        const auto& skeletalView = registry.view<Okay::CompSkeletalMesh, Transform>();
        for (entt::entity entity : skeletalView)
            ren.SumbitSkeletal(&skeletalView.get<Okay::CompSkeletalMesh>(entity), &skeletalView.get<Transform>(entity));
#endif
    
        const auto& lightView = registry.view<PointLight, Transform>();
        for (entt::entity entity : lightView)
            renderer.submitPointLight(lightView.get<PointLight>(entity), lightView.get<Transform>(entity));
    }
    
    void Scene::end()
    {
    
    }
}
