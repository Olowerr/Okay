#include "Engine.h"

Okay::Engine::Engine()
	:renderer(Renderer::Get()), assets(Assets::Get()), dx11(DX11::Get())
{
	std::ofstream writer(SceneDecleration.c_str, std::ios::binary, std::ios::trunc);
	const UINT NumScenes = 1;

	writer.write((const char*)&NumScenes, 4);


	/*first.AddComponent<Okay::MeshComponent>().mesh = Assets::GetMesh("gob.okayAsset");
	second.AddComponent<Okay::MeshComponent>().mesh = Assets::GetMesh("gob.okayAsset");

	first.GetComponent<Okay::TransformComponent>().SetPosition({ 3.f, 0.f, 0.f });
	second.GetComponent<Okay::TransformComponent>().SetPosition({ -3.f, 0.f, 0.f });*/


	writer.close();
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
	return false;
}

bool Okay::Engine::LoadScene(UINT scenIndex)
{
	return false;

}