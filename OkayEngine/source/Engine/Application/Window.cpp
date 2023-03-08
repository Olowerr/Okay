#include "Window.h"
#include "Input/Input.h"
#include "imgui/imgui_impl_win32.h"
#include "Engine/DirectX/DX11.h"

#include <windowsx.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::unordered_map<HWND, Window*> Window::windows;

Window::Window(uint32_t width, uint32_t height, const wchar_t* windowName, uint32_t renderTexFlags)
	:open(false), msg(), swapChain(nullptr)
{
	WNDCLASS winClass = {};
	winClass.lpfnWndProc = WindowProc;
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
	windows.insert({ hWnd, this });
	show();

	createRenderTexture(renderTexFlags == Okay::INVALID_UINT ? Okay::RenderTexture::B_RENDER : renderTexFlags);
}

Window::~Window()
{
	DestroyWindow(hWnd);
	windows.erase(hWnd);

	UnregisterClass(L"WinClass", GetModuleHandle(NULL));

	renderTexture.shutdown();
	DX11_RELEASE(swapChain);
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

void Window::setName(const wchar_t* name)
{
	SetWindowText(hWnd, name);
}

HWND Window::getHWnd() const
{
	return hWnd;
}

glm::uvec2 Window::getDimensions() const
{
	RECT rect{};
	GetClientRect(hWnd, &rect);
	return glm::uvec2(uint32_t(rect.right), uint32_t(rect.bottom));
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
		open = false;
}

bool Window::fileExplorerSelectFile(std::string& output)
{
	char fileName[MAX_FILENAME_LENGTH]{};

	OPENFILENAMEA ofn{};
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_FILENAME_LENGTH;
	ofn.lpstrFilter = "All Files\0*.*";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_NOCHANGEDIR;

	if (!GetOpenFileNameA(&ofn))
		return false;
	
	output = ofn.lpstrFile;

	return true;
}

void Window::createRenderTexture(uint32_t flags)
{
	DX11::createSwapChain(&swapChain, hWnd, 
		(CHECK_BIT(flags, Okay::RenderTexture::BitPos::B_RENDER)		? DXGI_USAGE_RENDER_TARGET_OUTPUT	: 0u) |
		(CHECK_BIT(flags, Okay::RenderTexture::BitPos::B_SHADER_READ)	? DXGI_USAGE_SHADER_INPUT			: 0u) |
		(CHECK_BIT(flags, Okay::RenderTexture::BitPos::B_SHADER_WRITE)	? DXGI_USAGE_UNORDERED_ACCESS		: 0u) );

	OKAY_ASSERT(swapChain, "Failed creating swapchain");

	getAndSetBackBuffer(flags);
}

void Window::getAndSetBackBuffer(uint32_t flags)
{
	ID3D11Texture2D* backBuffer = nullptr;
	swapChain->GetBuffer(0u, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	OKAY_ASSERT(swapChain, "Failed getting backBuffer");

	renderTexture.create(backBuffer, flags);
	DX11_RELEASE(backBuffer);
}

LRESULT Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		Okay::Input::setKeyDown((Key)wParam);
		return 0;

	case WM_KEYUP:
		Okay::Input::setKeyUp((Key)wParam);
		return 0;

	case WM_SYSKEYDOWN:
		Okay::Input::setKeyDown((Key)wParam);
		return 0;
		
	case WM_SYSKEYUP:
		Okay::Input::setKeyUp((Key)wParam);
		return 0;

	case WM_MOUSEMOVE:
		Okay::Input::mouseXPos = (float)GET_X_LPARAM(lParam);
		Okay::Input::mouseYPos = (float)GET_Y_LPARAM(lParam);
		return 0;

	case WM_MOUSEWHEEL:
		Okay::Input::mouseWheelDir = GET_WHEEL_DELTA_WPARAM(wParam) * (1.f / 120.f);
		return 0;

	case WM_LBUTTONDOWN:
		Okay::Input::mouseLeft = true;
		return 0;

	case WM_LBUTTONUP:
		Okay::Input::mouseLeft = false;
		return 0;

	case WM_RBUTTONDOWN:
		Okay::Input::mouseRight = true;
		return 0;

	case WM_RBUTTONUP:
		Okay::Input::mouseRight = false;
		return 0;

	case WM_SIZE:
		Window::onResize(hWnd, wParam); // TODO: Only call when resizing is finished
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Window::onResize(HWND hWnd, WPARAM wParam)
{
	if (wParam == SIZE_MINIMIZED)
		return;

	auto it = windows.find(hWnd);
	OKAY_ASSERT(it != windows.end(), "Resized invalid window (?)");

	Window& window = *it->second;

	// Release all external references to the backBuffer before resizing the buffer
	const uint32_t flags = window.renderTexture.getFlags();
	window.renderTexture.shutdown();

	window.swapChain->ResizeBuffers(0u, 0u, 0u, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	window.getAndSetBackBuffer(flags);
}
