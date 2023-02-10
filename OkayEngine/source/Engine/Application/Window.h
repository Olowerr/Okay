#pragma once
#include "Engine/Okay/Okay.h"
#include "Engine/Graphics/RenderTexture.h"
#include "glm/glm.hpp"

#include <Windows.h>
#include <d3d11.h>
#include <unordered_map>

class Window
{
public:
	static const size_t MAX_FILENAME_LENGTH = 256ull;

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

	glm::uvec2 getDimensions() const;

	void show();
	void close();
	inline bool isOpen() const;
	void setName(const wchar_t* name);

	HWND getHWnd() const;
	void update();
	bool openFileExplorer(char* pOutput, size_t bufferSize);

	static inline Window* getActiveWindow();
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WindowProcChild(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	MSG msg;
	HWND hWnd;
	bool open;

	ID3D11Texture2D* backBuffer;
	IDXGISwapChain* swapChain;
	Okay::RenderTexture renderTexture;

private: // Window handling through HWND
	static std::unordered_map<HWND, Window*> windows;
	static void onResize(HWND hWnd, WPARAM wParam);
};

inline bool Window::isOpen() const { return open; }

inline Okay::RenderTexture& Window::getRenderTexture() { return renderTexture; }
inline const Okay::RenderTexture& Window::getRenderTexture() const { return renderTexture; }

inline void Window::clear() { renderTexture.clear(); }
inline void Window::clear(float* colours) { renderTexture.clear(colours); }
inline void Window::clear(const glm::vec4& colour) { renderTexture.clear(colour); }

inline void Window::present() { swapChain->Present(0u, 0u); }

inline Window* Window::getActiveWindow() 
{
	auto it = Window::windows.find(GetActiveWindow());
	return it != Window::windows.end() ? it->second : nullptr;
}
