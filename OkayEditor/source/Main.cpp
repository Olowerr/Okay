#include "Application/Editor.h"

int main(int argc, char* args[])
{
#ifndef DIST
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // !DIST

	const char* scene = argc == 1 ? "" : args[1];
	
	float a = 64.f;
	float scale = 1.f;

	for (int i = 0; i < 8; i++)
	{
		a *= 2;
		printf("%.7f | %.7f\n", a, scale);
		scale /= 2.f;
	}

	Editor editor(scene);
	editor.run();

	return 0;
}