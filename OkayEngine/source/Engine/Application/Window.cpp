#include "Window.h"
#include "Input/Input.h"
#include "imgui/imgui_impl_win32.h"
#include "Engine/DirectX/DX11.h"

#include <windowsx.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::unordered_map<HWND, Window*> Window::windows;

Window::Window(uint32_t width, uint32_t height, const wchar_t* windowName, uint32_t renderTexFlags, bool defaultWinProc)
	:open(false), msg(), swapChain(nullptr), backBuffer(nullptr)
{
	WNDCLASS winClass = {};
	winClass.lpfnWndProc = defaultWinProc ? DefWindowProc : WindowProc;
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

	if (renderTexFlags != ~0u)
	{
		createRenderTexture(renderTexFlags);
	}
}

Window::~Window()
{
	DestroyWindow(hWnd);
	windows.erase(hWnd);

	UnregisterClass(L"WinClass", GetModuleHandle(NULL));

	renderTexture.shutdown();
	DX11_RELEASE(backBuffer);
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
	wchar_t fileName[MAX_FILENAME_LENGTH]{};

	OPENFILENAME ofn{};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = fileName;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_FILENAME_LENGTH;
	ofn.lpstrFilter = L"All Files\0*.*";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_NOCHANGEDIR;

	if (!GetOpenFileName(&ofn))
		return false;
	
	const size_t bufferSize = wcslen(fileName);
	output.resize(bufferSize);
	wcstombs_s(nullptr, output.data(), bufferSize + 1ull, fileName, bufferSize);

	return true;
}

void Window::createRenderTexture(uint32_t flags)
{
	DXGI_SWAP_CHAIN_DESC desc{};
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.BufferCount = 1;
	desc.BufferUsage = 
		(CHECK_BIT(flags, Okay::RenderTexture::BitPos::B_RENDER)		? DXGI_USAGE_RENDER_TARGET_OUTPUT	: 0u) |
		(CHECK_BIT(flags, Okay::RenderTexture::BitPos::B_SHADER_READ)	? DXGI_USAGE_SHADER_INPUT			: 0u) |
		(CHECK_BIT(flags, Okay::RenderTexture::BitPos::B_SHADER_WRITE)	? DXGI_USAGE_UNORDERED_ACCESS		: 0u);

	desc.BufferDesc.Width = 0u; // 0u defaults to the window dimensions
	desc.BufferDesc.Height = 0u;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.RefreshRate.Numerator = 0u;
	desc.BufferDesc.RefreshRate.Denominator = 1u;

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	desc.OutputWindow = hWnd;
	desc.Windowed = true;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ID3D11Device* pDevice = DX11::get().getDevice();
	IDXGIDevice* idxDevice = nullptr;
	IDXGIAdapter* adapter = nullptr;
	IDXGIFactory* factory = nullptr;

	pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&idxDevice);
	idxDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&adapter);
	adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);

	factory->CreateSwapChain(pDevice, &desc, &swapChain);
	DX11_RELEASE(idxDevice);
	DX11_RELEASE(adapter);
	DX11_RELEASE(factory);

	OKAY_ASSERT(swapChain, "Failed creating swapchain");

	swapChain->GetBuffer(0u, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	OKAY_ASSERT(swapChain, "Failed creating backBuffer");

	renderTexture.create(backBuffer, flags);
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

void Window::onResize(HWND hWnd, WPARAM wParam)
{
	if (wParam == SIZE_MINIMIZED)
		return;

	auto it = windows.find(hWnd);
	OKAY_ASSERT(it != windows.end(), "Resized invalid window??");

	Window& window = *it->second;
	if (!window.swapChain)
		return;

	// Release all external references to the backBuffer before resizing the buffer
	const uint32_t flags = window.renderTexture.getFlags();
	window.renderTexture.shutdown();
	DX11_RELEASE(window.backBuffer);

	window.swapChain->ResizeBuffers(0u, 0u, 0u, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	window.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&window.backBuffer);
	OKAY_ASSERT(window.backBuffer, "Failed to recreate backbuffer");
	
	// Recreate all views
	window.renderTexture.create(window.backBuffer, flags);
}
