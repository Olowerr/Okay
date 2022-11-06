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
	void setName(std::wstring_view name);

	HWND getHWnd() const;
	void update();
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	MSG msg;
	HWND hWnd;

	bool open;
};