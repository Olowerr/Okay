#include "Application/TerrainEditor.h"

int main(int argc, char* args[])
{
#ifndef DIST
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // !DIST

	TerrainEditor editor;
	editor.run();

	return 0;
}