#pragma once
#include "Header.h"

class B
{
public:
	B() = default;

	int q = 0;
	A* a;

	void foo() { int hello = 0; }
};