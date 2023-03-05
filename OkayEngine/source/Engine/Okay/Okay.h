#pragma once

#include <cassert>
#include <comdef.h>
#include <string>

//#include "glm/glm.hpp"


#ifdef DIST
#define OKAY_ASSERT(condition, devMsg) 
#else
// TODO: Kinda uneccessary to use a MessageBox if __debugbreak is being used anyway...
#define OKAY_ASSERT(condition, devMsg)\
if (!(condition))\
{\
	const std::wstring message = std::wstring(L"(" L#condition L") was false. Location: " __FILE__ ", Line: ") +\
			 std::to_wstring(__LINE__) + std::wstring("\nFunction: " __FUNCTIONW__); \
	MessageBoxW(NULL, (message + L"\n\nMessage: " devMsg).c_str(), L"ERROR", MB_OK);\
	__debugbreak();\
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

#define ENGINE_RESOURCES_PATH "../OkayEngine/engine_resources/"
#define SHADER_BIN_PATH ENGINE_RESOURCES_PATH "shaders/bin/"

namespace Okay
{
	constexpr uint32_t INVALID_UINT = ~0u;

	bool readBinary(std::string_view binName, std::string& output);
}