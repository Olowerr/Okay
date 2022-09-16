#include "Application.h"

bool Application::bruh = false;

Application::Application()
	:hWnd(nullptr)
{
}

Application::~Application()
{
	Okay::Editor::Destroy(); // Temp moved out of #ifdef
#ifdef EDITOR
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


	VERIFY(Okay::Editor::Create()); // Temp moved out of #ifdef
#ifdef EDITOR
#endif // EDITOR

	return true;
}

#include <thread>

void Application::Run()
{
	bruh = true;
	using namespace Okay;

	Engine::StartScene(); // Temp moved out of #ifndef
#ifndef EDITOR
#endif

	std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point b = std::chrono::system_clock::now();


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

		static double time = 10.0;
		static bool use = false;
		if (ImGui::Begin("Frames"))
		{
			ImGui::InputDouble("ms", &time, 0.1);
			ImGui::Checkbox("use", &use);
		}
		ImGui::End();

		if (use)
		{
			a = std::chrono::system_clock::now();
			std::chrono::duration<double, std::milli> work_time = a - b;

			if (work_time.count() < time)
			{
				std::chrono::duration<double, std::milli> delta_ms(time - work_time.count());
				auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
				std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
			}

			b = std::chrono::system_clock::now();
		}


#ifdef EDITOR
		Editor::Update();
#else
#endif
		Engine::Update(); // Temp moved out of #else


		Engine::Render();

#ifdef EDITOR
		Editor::EndFrame();
#endif
		Engine::EndFrame();
	}

	Engine::EndScene(); // Temp moved out of #ifndef
#ifndef EDITOR
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

	case WM_MOUSEMOVE:
		if (bruh) Okay::Engine::UpdateMouse();
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
