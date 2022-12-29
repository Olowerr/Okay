#pragma once
#include <Windows.h>
#include "Engine/Okay/Okay.h"

class Window
{
public:
	Window(uint32_t width, uint32_t height, const wchar_t* windowName, bool defaultWinProc = false);
	~Window();
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;
	
	void show();
	void close();
	bool isOpen() const;
	void setName(const wchar_t* name);

	HWND getHWnd() const;
	void update();
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WindowProcChild(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	MSG msg;
	HWND hWnd;

	bool open;
};