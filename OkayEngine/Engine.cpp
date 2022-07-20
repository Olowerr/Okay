#include "Engine.h"

const Okay::String Okay::Engine::SceneDecleration = "../Content/Scenes/SceneDecleration.okayDec";

Okay::Engine::Engine()
{
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
	
}

Okay::Engine::~Engine()
{
}

void Okay::Engine::Initialize()
{
	Get().assets.SetUp();

}

void Okay::Engine::NewFrame()
{
	Get().renderer.NewFrame();
	DX11::Get().NewFrame();
}

void Okay::Engine::EndFrame()
{
	DX11::Get().EndFrame();
}

void Okay::Engine::Update()
{
	Get().activeScene->Update();
}

bool Okay::Engine::SaveCurrentScene()
{
	std::ofstream writer(SceneDecleration.c_str, std::ios::binary | std::ios::trunc);
	VERIFY(writer);

	entt::registry& registry = Get().activeScene->GetRegistry();

	const UINT NumEntities = (UINT)registry.size();
	writer.write((const char*)&NumEntities, sizeof(UINT));


	//  --- Temp ---
	const auto& group = registry.group<Okay::CompMesh, Okay::CompTransform>();
	const UINT NumComp = 2;
	Okay::Components type;
	for (auto& entity : group)
	{
		const Okay::String& meshName = group.get<Okay::CompMesh>(entity).mesh->GetName();
		const Okay::CompTransform& transform = group.get<Okay::CompTransform>(entity);

		writer.write((const char*)&NumComp, sizeof(UINT));

		// Write Mesh
		type = Okay::Components::Mesh;
		writer.write((const char*)&type, sizeof(Okay::Components));
		writer.write((const char*)&meshName, sizeof(Okay::String));

		// Write Transform
		type = Okay::Components::Transform;
		writer.write((const char*)&type, sizeof(Okay::Components));
		writer.write((const char*)&transform.position, sizeof(Okay::Float3));
		writer.write((const char*)&transform.rotation, sizeof(Okay::Float3));
		writer.write((const char*)&transform.scale, sizeof(Okay::Float3));
	}

	writer.close();
	return true;
}

bool Okay::Engine::LoadScene(const Okay::String& sceneName)
{
	Get().activeScene.release();
	Get().activeScene = std::make_unique<Scene>(Get().renderer);

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

		auto& mat = entity.AddComponent<CompMesh>(meshName.c_str);
		mat.AssignMaterial(0, Get().assets.GetMaterial("goombaMat"));
		break;
	}

	case Components::Transform: // All entities get a transform component on creation
	{
		auto& transform = entity.GetComponent<CompTransform>();
		
		// Could read all three at the same time but that assums they're after one another in CompTransform
		reader.read((char*)&transform.position, sizeof(Okay::Float3));
		reader.read((char*)&transform.rotation, sizeof(Okay::Float3));
		reader.read((char*)&transform.scale, sizeof(Okay::Float3));
		
		transform.CalcMatrix();

		break;
	}
	}
}