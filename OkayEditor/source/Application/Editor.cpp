#include "Editor.h"


Editor::Editor(std::string_view startScene)
	:Application(L"Okay Editor"), scene(renderer)
{
	content.importFile("C:/Users/olive/source/repos/Okay/OkayEditor/resources/amogus.fbx");
	Okay::Entity entity = scene.createEntity();
	entity.addComponent<Okay::MeshComponent>(0u);
}

Editor::~Editor()
{
}
   
void Editor::run()
{
	DX11& dx11 = DX11::getInstance();

	scene.start();
	while (window.isOpen())
	{
		// New frame
		dx11.clear();
		renderer.newFrame();

		// Update
		window.update();
		scene.update();

		// Submit & render
		scene.submit();
		renderer.render();

		// Present
		dx11.present();
	} 
	scene.end();
}
