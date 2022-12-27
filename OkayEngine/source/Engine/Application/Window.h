#pragma once
#include <Windows.h>
#include "Engine/Okay/Okay.h"

class Window
{
public:
	Window(uint32_t width, uint32_t height, bool open = true);
	~Window();
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;
	
	void show();
	void close();
	bool isOpen() const;
	void setName(std::wstring_view name);

	HWND getHWnd() const;
	void update();
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	MSG msg;
	HWND hWnd;

	bool open;
};