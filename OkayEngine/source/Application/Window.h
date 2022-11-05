#pragma once
#include <Windows.h>
#include <string>

class Window
{
public:
	Window(std::wstring_view name, uint32_t width, uint32_t height);
	~Window();
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;

	void show();
	void close();
	bool isOpen() const;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND hWnd;
	HINSTANCE hInst;

	bool open;
};