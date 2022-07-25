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

void Okay::Engine::ResizeScreen()
{
	DX11::Get().ResizeBackBuffer();
	Get().renderer.Resize();
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

	// Might change
	const UINT NumEntities = (UINT)registry.alive();
	writer.write((const char*)&NumEntities, sizeof(UINT));


	//  --- Temp ---
	const auto& group = registry.group<Okay::CompMesh, Okay::CompTransform>();
	const UINT NumComp = 2;
	Okay::Components type;

	for (auto& entity : group)
	{
		if (!registry.valid(entity))
			continue;

		Okay::CompMesh& mesh = group.get<Okay::CompMesh>(entity);
		Okay::CompTransform& transform = group.get<Okay::CompTransform>(entity);

		writer.write((const char*)&NumComp, sizeof(UINT));

		// Write Mesh
		type = Okay::Components::Mesh;
		writer.write((const char*)&type, sizeof(Okay::Components));
		mesh.WritePrivateData(writer);

		// Write Transform
		type = Okay::Components::Transform;
		writer.write((const char*)&type, sizeof(Okay::Components));
		transform.WritePrivateData(writer);
	}

	writer.close();
	return true;
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

	Get().activeScene->Start();
	return true;
}

bool Okay::Engine::LoadScene(UINT sceneIndex)
{
	Get().activeScene->Start();
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
		entity.AddComponent<CompMesh>(reader);
		break;
	}

	case Components::Transform: 
	{
		// All entities get a transform component on creation
		entity.GetComponent<CompTransform>().ReadPrivateData(reader);
		break;
	}
	}
}