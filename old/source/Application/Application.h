#pragma once

#include "Engine/Engine.h"
#include "Editor/Editor.h"

class Application
{
public:
	Application();
	~Application();
	void Shutdown();

	bool Initiate();

	void Run();

private: // Window
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool InitiateWindow();
	HWND hWnd;
};