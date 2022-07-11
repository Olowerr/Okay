#define NOMINMAX

#include "System.h"

#include "ResourceManager.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	System system;

	if (!system.Initiate())
		return -1;

	Assets::Get().AddMesh("a/a/a/cube.fbx");
	Assets::Get().WriteDeclaration();
	Assets::Get().ReadDeclaration();
	
	
	//system.Run();
	system.Shutdown();

	return 0;
}