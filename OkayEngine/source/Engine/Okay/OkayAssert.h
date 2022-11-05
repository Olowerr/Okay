#pragma once
#include <cassert>
#include <comdef.h>

#ifdef DIST
#define OKAYASSERT(X)
#else
#define OKAYASSERT(X)\
if (!(X))\
MessageBoxW(NULL, L#X, L"ERROR", MB_OK)
#endif // DIST
