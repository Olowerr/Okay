#define NOMINMAX

#include "System.h"

/*

	Forward Decleration:

	headerFile.h
		// Don't include "Entity.h"
		struct Entity; // <- Forward declaration

		struct Scene
		{
			static Entity Create(); // DON'T DEFINE HERE OR INLINE. DEFINE IN .CPP
		};

	sourceFile.cpp
		#include "headerFile.h"
		#include "Entity.h" // Include Entity.h here

		Entity Scene::Create()
		{
			return Entity();
		}

*/

#include "Entity.h"

struct A 
{
	//A() = default;
	//~A() = default;
	//A(const A&) = default;
	//A(A&&) = default;
	//A& operator=(const A&) = default;


	int q = 0;
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Test Entity creation and component handling
	// Test Entity creation and component handling
	// Test Entity creation and component handling
	// Test Entity creation and component handling
	// Test Entity creation and component handling
	// Test Entity creation and component handling
	// Test Entity creation and component handling
	// Test Entity creation and component handling
	// Test Entity creation and component handling

	Scene myScene;
	Entity myEnt = myScene.CreateEntity();
	
	bool q = myEnt.HasComponent<A>();
	myEnt.AddComponent<A>();
	q = myEnt.HasComponent<A>();

	A& aRef = myEnt.GetComponent<A>();

	System system;

	if (!system.Initiate())
		return -1;
	
	system.Run();
	system.Shutdown();
	 
	return 0;
}