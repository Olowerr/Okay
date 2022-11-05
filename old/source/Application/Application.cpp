#include "Application.h"

Application::Application()
	:hWnd(nullptr)
{
}

Application::~Application()
{
#ifdef EDITOR
	Okay::Editor::Destroy(); 
#endif // EDITOR

	Shutdown();
}

void Application::Shutdown()
{
	DX11::Get().Shutdown();
}

bool Application::Initiate()
{
	std::srand((unsigned)time(0));

	InitiateWindow();
	Okay::Engine::Initialize(); // Should verify..

	VERIFY(Okay::Engine::LoadScene(""));


#ifdef EDITOR
	VERIFY(Okay::Editor::Create());
#endif

	return true;
}

void Application::Run()
{
	using namespace Okay;

#ifndef EDITOR
	Engine::StartScene(); 
#endif

	MSG msg{};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Engine::NewFrame();
#ifdef EDITOR
		Editor::NewFrame();
#endif

#ifdef EDITOR
		Editor::Update();
#else
		Engine::Update(); 
#endif


		Engine::Render();

#ifdef EDITOR
		Editor::EndFrame();
#endif
		Engine::EndFrame();
	}

#ifndef EDITOR
	Engine::EndScene(); 
#else
	Engine::SaveCurrent(); 
#endif

}

#ifdef EDITOR
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

LRESULT Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef EDITOR
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;
#endif

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		Okay::Engine::ResizeScreen();
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool Application::InitiateWindow()
{
	
	
	return true;
}
