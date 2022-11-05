#include "Engine.h"
#include <windowsx.h>

const Okay::String Okay::Engine::SceneDecleration = "Content/Scenes/SceneDecleration.okayDec";
bool Okay::Engine::keysDown[]{};
bool Okay::Engine::keysReleased[]{};

Okay::Engine::Engine()
	:deltaTime(), upTime()
{
	
	DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&dInput), NULL);

	dInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);
	DIMouse->SetDataFormat(&c_dfDIMouse);
	DIMouse->SetCooperativeLevel(GetHWindow(), DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	
	dInput->CreateDevice(GUID_SysKeyboard, &DIKeyboard, NULL);
	DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	DIKeyboard->SetCooperativeLevel(GetHWindow(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);


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
	
	printf("Engine Start\n");
}

Okay::Engine::~Engine()
{
	Get().DIMouse->Unacquire();
	Get().DIKeyboard->Unacquire();


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


	static DIMOUSESTATE currentState;
	static Okay::Engine& eng = Get();

	eng.DIMouse->Acquire();
	eng.DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &currentState);
	eng.lastState = currentState;

	eng.DIKeyboard->Acquire();

	static bool currentKeys[256] = {};
	eng.DIKeyboard->GetDeviceState(256, (LPVOID)&currentKeys);

	for (UINT i = 0; i < 256; i++)
		eng.keysReleased[i] = !currentKeys[i] && eng.keysDown[i];

	memcpy(eng.keysDown, currentKeys, 256);


	if (Get().keysReleased[DIK_E])
	{
		while (ShowCursor(FALSE) >= 0);
		Get().mouseLocked = true;
	}
	else if (Get().keysReleased[DIK_R])
	{
		while (ShowCursor(TRUE) <= 0);
		Get().mouseLocked = false;
	}

	if (Get().mouseLocked)
	{
		static RECT rect;
		GetWindowRect(GetHWindow(), &rect);
		SetCursorPos(rect.left + (rect.right - rect.left) / 2,
			rect.top + (rect.bottom - rect.top) / 2);
	}
}

void Okay::Engine::EndFrame()
{
	DX11::Get().EndFrame();

	Get().deltaTime = std::chrono::system_clock::now() - Get().frameStart;
	Get().upTime += Get().deltaTime;
}

void Okay::Engine::StartScene()
{
	Get().activeScene->Start();
}

void Okay::Engine::Update()
{
	Get().activeScene->Update();
}

void Okay::Engine::EndScene()
{
	Get().activeScene->End();
}

void Okay::Engine::Render()
{
	Get().activeScene->Submit();
	Get().renderer.Render();
}

bool Okay::Engine::SaveCurrentScene()
{
	std::ofstream writer(SceneDecleration.c_str, std::ios::binary | std::ios::trunc);
	VERIFY(writer);

	entt::registry& registry = activeScene->GetRegistry();

	// Might change
	const UINT NumEntities = (UINT)registry.alive();
	writer.write((const char*)&NumEntities, sizeof(UINT));


#if 1
	auto writeEntities = [&registry, &writer](auto entity)
	{

		UINT numComp = 0;
		COUNT_COMP(CompTransform);
		COUNT_COMP(CompMesh);
		COUNT_COMP(CompTag);
		COUNT_COMP(CompPointLight);

		writer.write((const char*)&numComp, sizeof(UINT));

		WRITE_DATA(CompTransform);
		WRITE_DATA(CompMesh);
		WRITE_DATA(CompTag);
		WRITE_DATA(CompPointLight);

	};

	registry.each(writeEntities);

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
	printf("Loading scene..\n");

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

	printf("Scene loaded\n");


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
		entity.AddComponent<CompMesh>().ReadPrivateData(reader);
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

	case Components::PointLight:
		entity.AddComponent<CompPointLight>().ReadPrivateData(reader);
		break;
	}

}