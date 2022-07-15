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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	System system;

	if (!system.Initiate())
		return -1;
	
	system.Run();
	system.Shutdown();
	 
	return 0;
}