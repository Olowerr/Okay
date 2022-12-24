#pragma once

#include <cassert>
#include <comdef.h>
#include <string>

#include "glm/glm.hpp"

#ifdef DIST
#define OKAY_ASSERT(condition, devMsg)\
if (!(condition))\
{\
	MessageBoxW(NULL, L"An internal error occoured.\nPlease try restarting.", L"ERROR", MB_OK);\
	exit(1);\
}0
#else
#define OKAY_ASSERT(condition, devMsg)\
if (!(condition))\
{\
	const std::wstring message(L"(" L#condition L") was false. Location: " __FILE__ ", Line: ");\
	MessageBoxW(NULL, (message + std::to_wstring(__LINE__) + L"\n\nMessage: " devMsg).c_str() , L"ERROR", MB_OK);\
	exit(1);\
}0

#define PRINT_VEC3_WNAME(vec) printf(#vec " - (%.3f, %.3f, %.3f)\n", vec.x, vec.y, vec.z);
#define PRINT_VEC3(vec)		  printf("(%.3f, %.3f, %.3f)\n", vec.x, vec.y, vec.z);

#endif // DIST

#define DX11_RELEASE(X) if (X) { X->Release(); X = nullptr; }
#define OKAY_VERIFY(X) if (!X) return false;