#include "ResourceManager.h"
#include "Engine.h"

Assets::Assets()
{
	// Make sure Okay::Engine::Get() is never called here
}

Assets::~Assets()
{

}

void Assets::SetUp()
{
	LoadAllMeshes();

	AddMesh("Goomba.obj");
	AddTexture("../Content/Images/quack.jpg");

	materials.insert({ "Default", std::make_shared<Okay::Material>() });
}

bool Assets::AddMesh(const std::string& filePath)
{
	// Fixes absolute paths
	std::string fileName = filePath.substr(filePath.find_last_of('/') + 1);
	fileName = fileName.substr(0, fileName.find_last_of('.')) + ".okayAsset";

	// Overrite old file / import new file
	Okay::VertexData data;
	std::string output[4];
	VERIFY(Importer::Load(filePath, data, output));

	if (output[1].size())
		AddTexture("../Content/Meshes/TempObjFbx/" + output[1]);
	if (output[2].size())
		AddTexture("../Content/Meshes/TempObjFbx/" + output[2]);
	if (output[3].size())
		AddTexture("../Content/Meshes/TempObjFbx/" + output[3]);

	Okay::MaterialDesc_Ptrs matDesc;
	matDesc.name = output[0];
	matDesc.baseColour = GetTexture(output[1]);
	matDesc.specular = GetTexture(output[2]);
	matDesc.ambient = GetTexture(output[3]);
	
	// Change material name
	AddMaterial(matDesc, output[0]);

	ReadDeclaration();
	
	// Check if the file exists
	bool found = false;
	for (auto& file : files)
	{
		if (file == fileName)
			found = true;
	}

	// Add to decleration if doesn't exist
	if (!found)
		files.emplace_back(fileName);

	WriteDeclaration();

	// Create the mesh
	std::shared_ptr<Okay::Mesh> mesh = std::make_shared<Okay::Mesh>(data, fileName);
	meshes[fileName] = mesh;
	
	return true;
}

std::shared_ptr<Okay::Mesh> Assets::GetMesh(const std::string& fileName)
{
	// Need to change
	if (meshes.find(fileName) == meshes.end())
		return std::make_shared<Okay::Mesh>(); 

	return meshes[fileName];
}

bool Assets::AddTexture(const std::string& filePath)
{
	const std::string& fileName = filePath.substr(filePath.find_last_of('/') + 1);

	if (textures.find(fileName) != textures.end())
		return false;

	textures.insert({ fileName, std::make_shared<Okay::Texture>(filePath) });
	return true;
}

std::shared_ptr<Okay::Texture> Assets::GetTexture(const std::string& fileName)
{
	// Need to change
	if (textures.find(fileName) == textures.end())
		return std::make_shared<Okay::Texture>(); 

	return textures[fileName];
}

bool Assets::AddMaterial(const Okay::MaterialDesc_Strs& matDesc, const std::string& materialName)
{
	if (materials.find(materialName) != materials.end())
		return false;

	materials.insert({ materialName, std::make_shared<Okay::Material>(matDesc) });
	return true;
}

bool Assets::AddMaterial(const Okay::MaterialDesc_Ptrs& matDesc, const std::string& materialName)
{
	if (materials.find(materialName) != materials.end())
		return false;

	materials.insert({ materialName, std::make_shared<Okay::Material>(matDesc) });
	return true;
}

std::shared_ptr<Okay::Material> Assets::GetMaterial(const std::string& materialName)
{
	if (materials.find(materialName) == materials.end())
		return std::make_shared<Okay::Material>();

	return materials[materialName];
}

bool Assets::LoadAllMeshes()
{
	VERIFY(ReadDeclaration());

	bool result = true, result2;
	for (const auto& file : files)
	{
		// File already loaded?
		if (meshes.find(file.c_str) != meshes.end())
			continue;

		// Attempt to load the file
		Okay::VertexData data;
		result2 = Importer::LoadOkayAsset(file.c_str, data);
		if (!result2)
		{
			result = false;
			continue;
		}

		// Create mesh and insert into the map
		std::shared_ptr<Okay::Mesh> mesh = std::make_shared<Okay::Mesh>(data, file);
		meshes.insert({ file.c_str, mesh });
	}

	return result;
}

bool Assets::ReadDeclaration()
{
	std::ifstream reader(DeclarationPath.c_str, std::ios::binary);
	VERIFY(reader);

	UINT NumFiles = 0;
	reader.read((char*)&NumFiles, sizeof(UINT));
	if (!NumFiles)
		return true;

	files.resize(NumFiles);

	const UINT ByteWidth = sizeof(Okay::String) * NumFiles;
	reader.read((char*)files.data(), ByteWidth);

	reader.close();
	return true;
}

bool Assets::WriteDeclaration()
{
	std::ofstream writer(DeclarationPath.c_str, std::ios::binary | std::ios::trunc);
	VERIFY(writer);

	const UINT NumFiles = (UINT)files.size();
	const UINT ByteWidth = sizeof(Okay::String) * NumFiles;

	writer.write((const char*)&NumFiles, sizeof(UINT));
	writer.write((const char*)files.data(), ByteWidth);

	writer.close();
	return true;
}
