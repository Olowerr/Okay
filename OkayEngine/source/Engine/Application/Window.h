#pragma once
#include "Engine/Okay/Okay.h"
#include "Engine/Graphics/RenderTexture.h"

#include <Windows.h>
#include <d3d11.h>


class Window
{
public:
	Window(uint32_t width, uint32_t height, const wchar_t* windowName, uint32_t renderTexFlags = ~0u, bool defaultWinProc = false);
	~Window();
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;
	
	void createRenderTexture(uint32_t flags);
	inline Okay::RenderTexture& getRenderTexture();
	inline const Okay::RenderTexture& getRenderTexture() const;
	inline void clear();
	inline void clear(float* colour);
	inline void clear(const glm::vec4& colour);
	inline void present();

	glm::ivec2 getDimensions() const;

	void show();
	void close();
	inline bool isOpen() const;
	void setName(const wchar_t* name);

	HWND getHWnd() const;
	void update();
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WindowProcChild(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	MSG msg;
	HWND hWnd;
	bool open;

	void createRenderTexture_Internal(uint32_t flags);
	ID3D11Texture2D* backBuffer;
	IDXGISwapChain* swapChain;
	Okay::RenderTexture renderTexture;
};

inline bool Window::isOpen() const
{
	return open;
}

inline Okay::RenderTexture& Window::getRenderTexture()
{
	return renderTexture;
}

inline const Okay::RenderTexture& Window::getRenderTexture() const
{
	return renderTexture;
}

inline void Window::clear()
{
	renderTexture.clear();
}

inline void Window::clear(float* colours)
{
	renderTexture.clear(colours);
}

inline void Window::clear(const glm::vec4& colour)
{
	renderTexture.clear(colour);
}

inline void Window::present()
{
	swapChain->Present(0u, 0u);
}
