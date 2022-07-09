#include "System.h"
//#include "Importer.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	//print();

	System system;

	if (!system.Initiate())
		return -1;

	system.Run();
	system.Shutdown();

	return 0;
}