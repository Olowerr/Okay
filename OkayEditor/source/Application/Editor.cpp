#include "Editor.h"
#include "Engine/Okay/OkayAssert.h"

Editor::Editor()
	:Application(L"Editor")
{
}

Editor::~Editor()
{
}

void Editor::run()
{
	while (window.isOpen())
	{
		window.update();

	}
}
