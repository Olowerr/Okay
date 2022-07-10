#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX

#include <d3d11.h>
#include <Windows.h>

#define VERIFY(X) if (!X) return false
#define VERIFY_HR(HR) if (FAILED(HR)) return false

#define WindowName L"Okay Engine"
#define GetHWindow() FindWindow(NULL, WindowName)

// temp
#define WIN_W 1600
#define WIN_H 900
#define WinRatio float(WIN_W) / float(WIN_H)

#define DX11_RELEASE(X) if (X) { X->Release(); X = nullptr; } 0
#define OkayDelete(X) if (X) {delete X; X = nullptr; } 0
#define OkayArrayDelete(X) if (X) {delete[]X; X = nullptr; } 0

#define ShaderPath "Shaders/"