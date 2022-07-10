#define NOMINMAX

#include "System.h"
#include <unordered_map>

#include "OkayImporter.h"

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

	void foo()
	{ }

	int values[10]{};
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Importer::VertexData data;
	Importer::Load("cube.fbx", data);

	/*std::unordered_map<std::string, std::shared_ptr<MyStruct>> meshes;

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

	ptr->values[0] = ptr->values[1] = ptr->values[2] = 1;*/

	/*int num = 10;
	Okay::Float3* verts = new Okay::Float3[num];
	verts[0].x = 1.f;
	verts[5].y = 5.f;
	verts[9].z = 9.f;


	std::unique_ptr<Okay::Float3[]> pop = std::make_unique<Okay::Float3[]>(num);
	memcpy(pop.get(), verts, sizeof(Okay::Float3) * num);

	delete[] verts;

	std::vector<Okay::Float3> hi;
	for (int i = 0; i < num; i++)
		hi.emplace_back(pop[i]);*/ 
	
	System system;

	if (!system.Initiate())
		return -1;

	system.Run();
	system.Shutdown();

	return 0;
}