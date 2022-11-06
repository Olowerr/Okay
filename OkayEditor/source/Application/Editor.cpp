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
	DX11& dx11 = DX11::getInstance();

	while (window.isOpen())
	{
		window.update();
		dx11.clear();


		dx11.present();
	} 
}
