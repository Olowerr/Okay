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

	Okay::Time::start();
	while (window.isOpen())
	{
		// New frame
		Application::newFrame();
		
		// Update
		scene.update();
		Okay::Time::setTimeScale(1.f);
		tra.rotation.y += Okay::Time::getDT();
		tra.calculateMatrix();
		tra.position = tra.forward() * -5.f;

		if (Okay::Input::isKeyDown(Keys::A))
			printf("A DOWN\n");
		if (Okay::Input::isKeyReleased(Keys::N))
			printf("N RELEASED\n");
		if (Okay::Input::isKeyPressed(Keys::Z))
			printf("Z PRESSED\n");


		// Submit & render
		scene.submit();
		renderer.render(scene.getMainCamera());

		// End frame
		Application::endFrame();
	} 
	scene.end();
}