#include "Editor.h"

Editor::Editor(std::string_view startScene)
	:Application(L"Okay Editor"), scene(renderer)
{
	content.importFile("C:/Users/olive/source/repos/Okay/OkayEditor/resources/texTest.fbx");
	content.importFile("C:/Users/olive/source/repos/Okay/OkayEditor/resources/axis.fbx");

	Okay::Entity entity = scene.createEntity();
	entity.addComponent<Okay::MeshComponent>(0u, 0u);
	Okay::Transform& tra = entity.getComponent<Okay::Transform>();
	tra.scale *= 2.f;

	Okay::Entity entity2 = scene.createEntity();
	entity2.addComponent<Okay::MeshComponent>(1u, 1u);
	Okay::Transform& tra2 = entity2.getComponent<Okay::Transform>();
	tra2.position.x = 5.f;

	Okay::Entity camera = scene.createEntity();
	camera.addComponent<Okay::Camera>();
	camera.getComponent<Okay::Transform>().position = glm::vec3(0.f, 0.f, 10.f);
	scene.setMainCamera(camera);
}

Editor::~Editor()
{
}
   
void Editor::run()
{

	DX11& dx11 = DX11::getInstance();
	Okay::Transform& tra = scene.getMainCamera().getComponent<Okay::Transform>();
	tra.rotation.x = glm::pi<float>() * 0.25f;
	scene.start();

	frameStart = std::chrono::system_clock::now();
	while (window.isOpen())
	{
		// New frame
		deltaTime = std::chrono::system_clock::now() - frameStart;
		frameStart = std::chrono::system_clock::now();

		dx11.clear();
		renderer.newFrame();

		// Update
		window.update();
		scene.update();

		tra.rotation.y += deltaTime.count();
		tra.calculateMatrix();
		tra.position = tra.forward() * -5.f;
		
		// Submit & render
		scene.submit();
		renderer.render(scene.getMainCamera());

		// Present
		dx11.present();
	} 
	scene.end();
}
