#include "Editor.h"

Editor::Editor(std::string_view scene)
	:Application(L"Okay Editor")
{
	dx11.initalize(&window);
}

Editor::~Editor()
{
}
   
void Editor::run()
{
	while (window.isOpen())
	{
		window.update();
		dx11.clear();


		dx11.present();
	} 
}
