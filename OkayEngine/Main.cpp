#include "System.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	System system;

	if (!system.Initiate())
		return -1;

	system.Run();
	system.Shutdown();

	return 0;
}