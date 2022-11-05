#include "Editor.h"

Editor::Editor(std::string_view scene)
	:Application(L"Okay Editor")
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
