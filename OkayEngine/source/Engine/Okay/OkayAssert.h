#pragma once
#include <cassert>
#include <comdef.h>

#ifdef DIST
#define OKAYASSERT(X)
#else
#define OKAYASSERT(X)\
if (!(X))\
MessageBoxW(NULL, L"(" L#X L") was false.", L"ERROR", MB_OK)
#endif // DIST
