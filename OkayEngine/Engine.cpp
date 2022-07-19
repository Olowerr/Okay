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

		entity.AddComponent<CompMesh>(meshName.c_str);
		break;
	}

	case Components::Transform: // All entities get a transform component on creation
	{
		Okay::Float3 transform[3]{};
		reader.read((char*)transform, sizeof(Okay::Float3) * 3);
		
		auto& cTransform = entity.GetComponent<CompTransform>();
		cTransform.position = transform[0];
		cTransform.rotation = transform[1];
		cTransform.scale = transform[2];
		cTransform.CalcMatrix();

		break;
	}
	}
}