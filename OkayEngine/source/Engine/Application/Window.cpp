#include "Window.h"
#include "Input/Input.h"
#include "imgui/imgui_impl_win32.h"
#include "Engine/DirectX/DX11.h"

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

//void Window::createRenderTexture(uint32_t flags)
//{
//	RECT rect{};
//	GetWindowRect(hWnd, &rect);
//	renderTexture.create(uint32_t(rect.right - rect.left), uint32_t(rect.bottom - rect.top), flags);
//	createRenderTexture_Internal(flags);
//}

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

glm::ivec2 Window::getDimensions() const
{
	RECT rect{};
	GetWindowRect(hWnd, &rect);
	return glm::ivec2(int(rect.right - rect.left), int(rect.bottom - rect.top));
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

bool Window::openFileExplorer(char* pOutput, size_t bufferSize)
{
	if (!pOutput || !bufferSize)
		return false;

	OPENFILENAME ofn{};

	wchar_t fileName[MAX_FILENAME_LENGTH]{};
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

	wcstombs_s(nullptr, pOutput, bufferSize, ofn.lpstrFile, bufferSize);
	return true;
}

void Window::createRenderTexture(uint32_t flags)
{
	DXGI_SWAP_CHAIN_DESC desc{};
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.BufferCount = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;

	desc.BufferDesc.Width = 0u; // 0u defaults to window dimensions
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

	ID3D11Device* pDevice = DX11::getInstance().getDevice();
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
		break;

	case WM_KEYUP:
		Okay::Input::setKeyUp((Key)wParam);
		break;

	case WM_SIZE:
		Window::onResize(hWnd, wParam);
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

void Window::onResize(HWND hWnd, WPARAM wParam)
{
	if (wParam == SIZE_MINIMIZED)
		return;

	auto it = windows.find(hWnd);
	OKAY_ASSERT(it != windows.end(), "Resized invalid window??");

	Window& window = *it->second;
	if (!window.swapChain)
		return;

	// Release all external references to the backBuffer before resizing it
	const uint32_t flags = window.renderTexture.getFlags();
	window.renderTexture.shutdown();
	DX11_RELEASE(window.backBuffer);

	window.swapChain->ResizeBuffers(0u, 0u, 0u, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	window.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&window.backBuffer);
	OKAY_ASSERT(window.backBuffer, "Failed to recreate backbuffer");
	
	// Recreate all views
	window.renderTexture.create(window.backBuffer, flags);
}
