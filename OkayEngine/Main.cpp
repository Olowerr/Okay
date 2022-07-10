#define NOMINMAX
#include "System.h"

#include <unordered_map>

struct MyStruct
{
	MyStruct()
	{
		int q = 0;
	}
	~MyStruct()
	{
		int q = 0;
	}

	int values[10]{};
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	std::unordered_map<std::string, std::shared_ptr<MyStruct>> meshes;

	meshes["0"] = std::make_shared<MyStruct>();
	std::shared_ptr<MyStruct> ptr = meshes["0"];

	meshes["1"];
	meshes["2"];
	meshes["3"];
	meshes["4"];
	meshes["5"];
	meshes["6"];
	meshes["7"];
	meshes["8"];
	meshes["9"];
	meshes["10"];
	meshes["11"];

	//ptr->values[0] = ptr->values[1] = ptr->values[2] = 1;


	System system;

	if (!system.Initiate())
		return -1;

	system.Run();
	system.Shutdown();

	return 0;
}