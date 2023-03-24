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

	Window(uint32_t width, uint32_t height, const wchar_t* windowName, uint32_t renderTexFlags = Okay::INVALID_UINT);
	~Window();
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;
	
	void createRenderTexture(uint32_t flags);
	inline Okay::Ref<Okay::RenderTexture> getRenderTexture();
	inline const Okay::Ref<Okay::RenderTexture> getRenderTexture() const;
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
	bool fileExplorerSelectFile(std::string& output);

	static inline Window* getActiveWindow();
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	MSG msg;
	HWND hWnd;
	bool open;

	IDXGISwapChain* pSwapChain;
	Okay::Ref<Okay::RenderTexture> renderTexture;

	void getAndSetBackBuffer(uint32_t flags);

private: // Window handling through HWND
	static std::unordered_map<HWND, Window*> windows;
	static void onResize(HWND hWnd, WPARAM wParam);
};

inline bool Window::isOpen() const { return open; }

inline Okay::Ref<Okay::RenderTexture> Window::getRenderTexture() { return renderTexture; }
inline const Okay::Ref<Okay::RenderTexture> Window::getRenderTexture() const { return renderTexture; }

inline void Window::clear() { renderTexture->clear(); }
inline void Window::clear(float* colours) { renderTexture->clear(colours); }
inline void Window::clear(const glm::vec4& colour) { renderTexture->clear(colour); }

inline void Window::present() { pSwapChain->Present(0u, 0u); }

inline Window* Window::getActiveWindow() 
{
	auto it = Window::windows.find(GetActiveWindow());
	return it != Window::windows.end() ? it->second : nullptr;
}
