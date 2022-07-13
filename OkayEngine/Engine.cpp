#include "Engine.h"

Okay::Engine::Engine()
	:renderer(Renderer::Get()), assets(Assets::Get()), dx11(DX11::Get())
{

}

Okay::Engine::~Engine()
{
}

void Okay::Engine::NewFrame()
{
	Get().dx11.NewFrame();
}

void Okay::Engine::EndFrame()
{
	Get().dx11.EndFrame();
}
