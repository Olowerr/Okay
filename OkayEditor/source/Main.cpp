#include "Application/Editor.h"

int main(int argc, char* args[])
{
	const char* scene = argc == 1 ? "" : args[1];

	Editor editor(scene);
	editor.run();

	return 0;
}