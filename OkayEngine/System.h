#pragma once

#include "Engine.h"
#include "Editor.h"

class System
{
public:
	System();
	~System();
	void Shutdown();

	bool Initiate();

	void Run();

private: // Window
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool InitiateWindow();
	HWND hWnd;
};