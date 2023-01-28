#pragma once

#include <cassert>
#include <comdef.h>
#include <string>

#include "glm/glm.hpp"

#ifdef DIST
#define OKAY_ASSERT(condition, devMsg) 
#else
#define OKAY_ASSERT(condition, devMsg)\
if (!(condition))\
{\
	const std::wstring message(L"(" L#condition L") was false. Location: " __FILE__ ", Line: ");\
	const std::wstring message2("\nFunction: " __FUNCTIONW__);\
	if (MessageBoxW(NULL, (message + std::to_wstring(__LINE__) + message2 + L"\n\nMessage: " devMsg).c_str() , L"ERROR", MB_RETRYCANCEL) == IDCANCEL)\
		exit(1);\
	assert(false);\
}0

#define PRINT_VEC3_WNAME(vec) printf(#vec " - (%.3f, %.3f, %.3f)\n", vec.x, vec.y, vec.z);
#define PRINT_VEC3(vec)		  printf("(%.3f, %.3f, %.3f)\n", vec.x, vec.y, vec.z);

#endif // DIST


#define DX11_RELEASE(X)		 if (X) { X->Release(); X = nullptr; }
#define OKAY_DELETE(X)		 if (X) { delete X;		X = nullptr; }
#define OKAY_DELETE_ARRAY(X) if (X) { delete[]X;	X = nullptr; }

#define OKAY_VERIFY(X)		if (!X) return false;
#define CHECK_BIT(X, pos)	((X) & 1<<(pos))
#define ARRAY_SIZE(X)		(sizeof(X) / sizeof(X[0]))

#define VEC2_GLM_TO_IMGUI(vec)	ImVec2((float)vec.x, (float)vec.y)
#define UNORM_TO_UCHAR(value)	unsigned char((value) * UCHAR_MAX)
#define UCHAR_TO_UNORM(value)	float((value) / (float)UCHAR_MAX)

namespace Okay
{
	constexpr uint32_t INVALID_UINT = ~0u;
}