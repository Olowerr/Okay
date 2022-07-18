#pragma once
#include "Header.h"

class A
{
public:
	A() = default;

	int q = 0;
	B* b;

	void foo() { int hello = 0; }
};