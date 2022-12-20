#pragma once

#include <cassert>
#include <comdef.h>
#include <string>

#include "glm/glm.hpp"

#ifdef DIST
#define OKAY_ASSERT(X)
#else
#define OKAY_ASSERT(condition, devMsg)\
if (!(condition))\
{\
	const std::wstring message(L"(" L#condition L") was false. Location: " __FILE__ ", Line: ");\
	MessageBoxW(NULL, (message + std::to_wstring(__LINE__) + L"\n\nDev message: " devMsg).c_str() , L"ERROR", MB_OK);\
	exit(EXIT_FAILURE);\
}0
#endif // DIST

#define DX11_RELEASE(X) if (X) { X->Release(); X = nullptr; }
#define OKAY_VERIFY(X) if (!X) return false;