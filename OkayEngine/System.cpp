#include "System.h"

System::System()
	:hWnd(nullptr)
{
}

System::~System()
{
	Shutdown();
}

void System::Shutdown()
{
	DX11::Get().Shutdown();
}

bool System::Initiate()
{
	VERIFY(InitiateWindow());
	DX11::Get(); // Sets up DX11 

	return true;
}

void System::Run()
{
	using namespace Okay;

	MSG msg{};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Engine::NewFrame();
		
		Engine::GetRenderer().Render();
		
		Engine::EndFrame();
	}

}

LRESULT System::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool System::InitiateWindow()
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
