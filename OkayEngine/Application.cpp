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
	InitiateWindow();
	Okay::Engine::Initialize(); // Should verify..

	VERIFY(Okay::Engine::LoadScene(""));

#ifdef EDITOR
	VERIFY(Okay::Editor::Create());
#endif // EDITOR

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
#endif // EDITOR

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

	case WM_KEYUP:
		Okay::Engine::SetKeyUp((UINT)wParam);
		return 0;

	case WM_KEYDOWN:
		Okay::Engine::SetKeyDown((UINT)wParam);
		return 0;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool Application::InitiateWindow()
{
	const wchar_t win_Class[] = L"WinClass";

	WNDCLASS winClass = {};
	winClass.lpfnWndProc = WindowProc;
	winClass.hInstance = GetModuleHandle(NULL);
	winClass.lpszClassName = win_Class;
	winClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClass(&winClass);

	RECT rect = {};
	rect.right = WIN_W;
	rect.bottom = WIN_H;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindowEx(0, win_Class, WindowName, WS_OVERLAPPEDWINDOW,
		0, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, GetModuleHandle(NULL), nullptr);

	if (!hWnd)
		return false;

	ShowWindow(hWnd, SW_SHOWDEFAULT);
	
	return true;
}
