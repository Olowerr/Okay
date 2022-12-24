#include "Window.h"
#include "Input/Input.h"

Window::Window(uint32_t width, uint32_t height)
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
	open = true;
	show();
}

Window::~Window()
{
}

void Window::show()
{
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

void Window::close()
{
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

HWND Window::getHWnd() const
{
	return hWnd;
}

void Window::update()
{
	Okay::Input::update();

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT)
		open = false;
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
