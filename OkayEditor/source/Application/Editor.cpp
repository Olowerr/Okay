#include "Editor.h"

#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/MeshComponent.h"

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
	camera.addComponent<Okay::PointLight>().intensnity = 3.f;
	scene.setMainCamera(camera);
}

Editor::~Editor()
{
}
   
void Editor::run()
{
	using namespace Okay;
	DX11& dx11 = DX11::getInstance();
	Transform& tra = scene.getMainCamera().getComponent<Transform>();
	tra.rotation.x = glm::pi<float>() * 0.25f;
	
	scene.start();
	Time::start();
	float timer = 0.f;

	Window win2(500u, 500u, false);
	while (window.isOpen())
	{
		// New frame
		Application::newFrame();
		
		// Update
		scene.update();
		Time::setTimeScale(1.f);
		tra.rotation.y += Time::getDT();
		tra.calculateMatrix();
		tra.position = tra.forward() * -5.f;

		if (Input::isKeyDown(Keys::A))
			printf("A DOWN\n");
		if (Input::isKeyReleased(Keys::N))
			printf("N RELEASED\n");
		if (Input::isKeyPressed(Keys::Z))
			printf("Z PRESSED\n");

		if (win2.isOpen())
			win2.update();

		if (Input::isKeyPressed(Keys::ONE))
			win2.show();
		if (Input::isKeyPressed(Keys::TWO))
			win2.close();


		//timer += Time::getApplicationDT();
		//if (timer >= 0.5f)
		//{
		//	timer -= 0.5f;
		//	printf("FPS: %.3f\n", 1.f / Time::getApplicationDT());
		//}

		// Submit & render
		scene.submit();
		renderer.render(scene.getMainCamera());

		// End frame
		Application::endFrame();
	} 
	scene.end();
}