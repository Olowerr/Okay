#pragma once

#include <cassert>
#include <comdef.h>
#include <string>

#include "glm/glm.hpp"

#ifdef DIST
#define OKAY_ASSERT(X)
#else
#define OKAY_ASSERT(X, msg)\
if (!(X))\
{\
	MessageBoxW(NULL, L"(" L#X L") was false.\n" msg, L"ERROR", MB_OK);\
	exit(EXIT_FAILURE);\
}0
#endif // DIST

#define DX11_RELEASE(X) if (X) { X->Release(); X = nullptr; }
#define OKAY_VERIFY(X) if (!X) return false;

using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

namespace Okay
{

	using Float2	= glm::vec2;
	using Float3	= glm::vec3;
	using Float4	= glm::vec4;
	using Float4x4	= glm::mat4;
}
