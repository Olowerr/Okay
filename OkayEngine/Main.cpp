#define NOMINMAX

#include "System.h"

#include "OkayImporter.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Importer::VertexData data;
	Importer::Load("", data);

	System system;

	if (!system.Initiate())
		return -1;

	system.Run();
	system.Shutdown();

	return 0;
}