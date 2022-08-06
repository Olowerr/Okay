#include "Engine.h"

const Okay::String Okay::Engine::SceneDecleration = "../Content/Scenes/SceneDecleration.okayDec";
bool Okay::Engine::keys[]{};

Okay::Engine::Engine()
	:deltaTime()
{
	printf("Engine Start\n");

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
	printf("Engine Shutdown\n");
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
	Get().frameStart = std::chrono::system_clock::now();

	Get().renderer.NewFrame();
	DX11::Get().NewFrame();
}

void Okay::Engine::EndFrame()
{
	DX11::Get().EndFrame();

	Get().deltaTime = std::chrono::system_clock::now() - Get().frameStart;
}

void Okay::Engine::Update()
{
	Get().activeScene->Update();
}

bool Okay::Engine::SaveCurrentScene()
{
	std::ofstream writer(SceneDecleration.c_str, std::ios::binary | std::ios::trunc);
	VERIFY(writer);

	entt::registry& registry = activeScene->GetRegistry();

	// Might change
	const UINT NumEntities = (UINT)registry.alive();
	writer.write((const char*)&NumEntities, sizeof(UINT));

											
#if 0
	Okay::Components type;
	registry.each([&registry, &writer](auto entity)
	{
		
		if (auto* ptr = registry.try_get<Okay::CompTransform>(entity))
			foo(ptr, writer);

		if (auto* ptr = registry.try_get<Okay::CompMesh>(entity))
			foo(ptr, writer);

		if (auto* ptr = registry.try_get<Okay::CompTag>(entity))
			foo(ptr, writer);

		
	});

#else

	//  --- Temp ---
	const auto& group = registry.group<Okay::CompMesh, Okay::CompTransform, Okay::CompTag>();
	const UINT NumComp = 3;
	Okay::Components type;

	for (auto& entity : group)
	{
		if (!registry.valid(entity))
			continue;
		
		const auto& [mesh, transform, tag] = group.get(entity);
		
		writer.write((const char*)&NumComp, sizeof(UINT));

		// Write Mesh
		type = Okay::Components::Mesh;
		writer.write((const char*)&type, sizeof(Okay::Components));
		mesh.WritePrivateData(writer);

		// Write Transform
		type = Okay::Components::Transform;
		writer.write((const char*)&type, sizeof(Okay::Components));
		transform.WritePrivateData(writer);

		// Write Tag
		type = Okay::Components::Tag;
		writer.write((const char*)&type, sizeof(Okay::Components));
		tag.WritePrivateData(writer);

	}

#endif

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

	case Components::Tag:
		// All entities get a transform component on creation
		entity.GetComponent<CompTag>().ReadPrivateData(reader);
		break;
	}
}