#include "Application/Editor.h"

int main(int argc, char* args[])
{
#ifndef DIST
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // !DIST

	const char* scene = argc == 1 ? "" : args[1];
	
	Editor editor(scene);
	editor.run();

	return 0;
}