#include "Scene.h"
#include "Engine.h"

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

    entities.emplace_back(entity);

    return entity;
}

void Scene::Start()
{
    entities.reserve(10);
}

void Scene::Update()
{
#ifdef _IMGUI

    if (ImGui::Begin("Entity"))
    {
        if (ImGui::Button("Add"))
        {
            CreateEntity().AddComponent<Okay::CompMesh>("cube.OkayAsset");
            index = (int)entities.size() - 1;
        }

        ImGui::InputInt("Entity Index", &index);
 
        // Clamp Index
        index = index >= (int)entities.size() ? (int)entities.size() - 1 : index < 0 ? 0 : index;
        
        if (!entities.size())
        {
            index = -1;
            ImGui::End();
            return;
        }

        if (ImGui::Button("Remove"))
        {
            registry.destroy(entities.at(index));
            entities.erase(entities.begin() + index);

            index = index >= (int)entities.size() ? (int)entities.size() - 1 : index < 0 ? 0 : index;
            if (!entities.size())
            {
                index = -1;
                ImGui::End();
                return;
            }
        }

        auto& tra = entities.at(index).GetComponent<Okay::CompTransform>();

        ImGui::DragFloat3("Position", &tra.position.x, 0.01f);
        ImGui::DragFloat3("Rotation", &tra.rotation.x, 0.01f);
        ImGui::DragFloat3("Scale",    &tra.scale.x, 0.01f);
        tra.CalcMatrix();
    }
    ImGui::End();
#endif // _IMGUI


    const auto& group = registry.group<Okay::CompMesh, Okay::CompTransform>();

    Renderer& ren = Okay::Engine::GetRenderer();
    for (auto& entity : group)
        ren.Submit(&group.get<Okay::CompMesh>(entity), &group.get<Okay::CompTransform>(entity));
}
