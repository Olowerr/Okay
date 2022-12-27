#include "Window.h"
#include "Input/Input.h"

Window::Window(uint32_t width, uint32_t height, bool open)
	:child(nullptr)
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
	rect.right = (LONG)width;
	rect.bottom = (LONG)height;

	RECT desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);
	LONG xDiff = desktop.right - rect.right;
	LONG yDiff = desktop.bottom - rect.bottom;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindowEx(0, win_Class, L"Okay Engine", WS_OVERLAPPEDWINDOW,
		xDiff / 2, yDiff / 4, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, GetModuleHandle(NULL), nullptr);

	OKAY_ASSERT(hWnd != nullptr, "Failed creating window");

	if (open)
		show();
}

Window::~Window()
{
	DestroyWindow(hWnd);
	UnregisterClass(L"WinClass", GetModuleHandle(NULL));

	if (child)
	{
		DestroyWindow(child);
		UnregisterClass(L"WinClass2", GetModuleHandle(NULL));
	}
}

void Window::show()
{
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	open = true;
}

void Window::close()
{
	CloseWindow(hWnd);
	open = false;
}

bool Window::isOpen() const
{
	return open;
}

void Window::setName(std::wstring_view name)
{
	SetWindowText(hWnd, name.data());
}

void Window::createChild()
{
	printf(__FUNCTION__ "%p\n", child);
	if (child)
		return;

	const wchar_t win_Class[] = L"WinClass2";

	WNDCLASS winClass = {};
	winClass.lpfnWndProc = WindowProcChild;
	winClass.hInstance = GetModuleHandle(NULL);
	winClass.lpszClassName = win_Class;
	winClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&winClass);

	child = CreateWindowEx(0, win_Class, L"Okay Engine Child", WS_OVERLAPPEDWINDOW,0,0, 500, 500,
		hWnd, nullptr, GetModuleHandle(NULL), nullptr);

	OKAY_ASSERT(child != nullptr, "Failed creating child window");

	ShowWindow(child, SW_SHOWDEFAULT);
}

HWND Window::getHWnd() const
{
	return hWnd;
}

void Window::update()
{
	Okay::Input::update();

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT)
	{
		open = false;
		CloseWindow(hWnd);
	}
	return;

	// Hmm
	if (!child)
		return;

	if (PeekMessage(&msg, child, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT)
	{
		DestroyWindow(child);
		UnregisterClass(L"WinClass2", GetModuleHandle(NULL));
		child = nullptr;
		printf(__FUNCTION__ "%p\n", child);
	}
}

LRESULT Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		
	case WM_KEYDOWN:
		Okay::Input::setKeyDown((Keys)wParam);
		break;

	case WM_KEYUP:
		Okay::Input::setKeyUp((Keys)wParam);
		break;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Window::WindowProcChild(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//switch (message)
	//{
	//case WM_DESTROY:
	//	PostQuitMessage(0);
	//	return 0;
	//
	//case WM_CLOSE:
	//	PostQuitMessage(0);
	//	return 0;
	//
	//}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
