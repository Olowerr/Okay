#include "Engine.h"
#include <typeinfo>

const Okay::String Okay::Engine::SceneDecleration = "../Content/Scenes/SceneDecleration.okayDec";

Okay::Engine::Engine()
	:renderer(Renderer::Get()), assets(Assets::Get()), dx11(DX11::Get())
{

	std::ofstream writer(SceneDecleration.c_str, std::ios::binary | std::ios::trunc);

	UINT num = 2;
	writer.write((const char*)&num, 4);
	
	Components asd = Components::Mesh;
	Okay::String mesh = "gob.okayAsset";
	Components asd2 = Components::Transform;
	Okay::Float3 t[3]{ {3.f, 0.f, 0.f}, {}, {1.f, 1.f, 1.f} };

	writer.write((const char*)&num, 4);
	
	writer.write((const char*)&asd, sizeof(asd));
	writer.write((const char*)mesh.c_str, sizeof(mesh));

	writer.write((const char*)&asd2, sizeof(asd2));
	writer.write((const char*)t, sizeof(t));
	

	t[0].x = -3.f;
	mesh = "cube.okayAsset";
	writer.write((const char*)&num, 4);
	writer.write((const char*)&asd, sizeof(asd));
	writer.write((const char*)mesh.c_str, sizeof(mesh));

	writer.write((const char*)&asd2, sizeof(asd2));
	writer.write((const char*)t, sizeof(t));

	writer.close();



	
	/*
		Currently reading the scene wrong
		Also can't create TransformComponent2 with ENTT for whatever reason??
		Currently reading the scene wrong
		Also can't create TransformComponent2 with ENTT for whatever reason??
		Currently reading the scene wrong
		Also can't create TransformComponent2 with ENTT for whatever reason??
		Currently reading the scene wrong
		Also can't create TransformComponent2 with ENTT for whatever reason??
		Currently reading the scene wrong
		Also can't create TransformComponent2 with ENTT for whatever reason??
		Currently reading the scene wrong
		Also can't create TransformComponent2 with ENTT for whatever reason??
		Currently reading the scene wrong
		Also can't create TransformComponent2 with ENTT for whatever reason??
		Currently reading the scene wrong
		Also can't create TransformComponent2 with ENTT for whatever reason??
	
	*/



	/*
	
	//NumScenes

		NumEntity
		Entity
			NumComp
			Components::Type
				data
			Components::Type
				data
			Components::Type
				data
		Entity
			NumComp
			Components::Type
				data
			Components::Type
				data
	*/




	/*first.AddComponent<Okay::MeshComponent>().mesh = Assets::GetMesh("gob.okayAsset");
	second.AddComponent<Okay::MeshComponent>().mesh = Assets::GetMesh("gob.okayAsset");

	first.GetComponent<Okay::TransformComponent2>().SetPosition({ 3.f, 0.f, 0.f });
	second.GetComponent<Okay::TransformComponent2>().SetPosition({ -3.f, 0.f, 0.f });*/


}

Okay::Engine::~Engine()
{
}

void Okay::Engine::NewFrame()
{
	Get().renderer.NewFrame();
	Get().dx11.NewFrame();
}

void Okay::Engine::EndFrame()
{
	Get().dx11.EndFrame();
}

bool Okay::Engine::LoadScene(const Okay::String& sceneName)
{
	Get().activeScene.release();
	Get().activeScene = std::make_unique<Scene>();

	std::ifstream reader(SceneDecleration.c_str, std::ios::binary);
	VERIFY(reader);
	
	UINT NumEntities = 0;
	reader.read((char*)&NumEntities, sizeof(UINT));

	for (UINT i = 0; i < NumEntities; i++)
	{
		Entity ent = Get().activeScene->CreateEntity();
		ReadEntity(ent, reader);
	}

	return true;
}

bool Okay::Engine::LoadScene(UINT sceneIndex)
{
	return false;
}

void Okay::Engine::ReadEntity(Entity& entity, std::ifstream& reader)
{
	UINT numComp = 0;
	reader.read((char*)&numComp, sizeof(UINT));

	Components type = Components::None;
	for (UINT i = 0; i < numComp; i++)
	{
		reader.read((char*)&type, sizeof(Components));

		ReadComponentData(entity, type, reader);
	}
}

void Okay::Engine::ReadComponentData(Entity& entity, Components type, std::ifstream& reader)
{
	switch (type)
	{
	default:
		return;

	case Components::Mesh:
	{
		Okay::String meshName;
		reader.read(meshName.c_str, sizeof(Okay::String));

		entity.AddComponent<CompMesh>(meshName);
		break;
	}

	case Components::Transform: // All entities get a transform component on creation
	{
		Okay::Float3 transform[3];
		reader.read((char*)transform, sizeof(Okay::Float3) * 3);
		
		auto& cTransform = entity.GetComponent<CompTransform>();
		cTransform.position = transform[0];
		cTransform.rotation = transform[1];
		cTransform.scale = transform[2];

		break;
	}
	}
}