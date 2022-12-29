#include "Window.h"
#include "Input/Input.h"

Window::Window(uint32_t width, uint32_t height, const wchar_t* windowName, bool defaultWinProc)
	:open(false), msg()
{
	WNDCLASS winClass = {};
	winClass.lpfnWndProc = defaultWinProc ? DefWindowProcW : WindowProc;
	winClass.hInstance = GetModuleHandle(NULL);
	winClass.lpszClassName = windowName;
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
	hWnd = CreateWindowEx(0, windowName, windowName, WS_OVERLAPPEDWINDOW,
		xDiff / 2, yDiff / 4, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, GetModuleHandle(NULL), nullptr);

	OKAY_ASSERT(hWnd != nullptr, "Failed creating window");

	show();
}

Window::~Window()
{
	DestroyWindow(hWnd);
	UnregisterClass(L"WinClass", GetModuleHandle(NULL));
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

void Window::setName(const wchar_t* name)
{
	SetWindowText(hWnd, name);
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
