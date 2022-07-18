#include "ResourceManager.h"

Assets::Assets()
{
	LoadAll();
	
	//AddMesh("gob.obj");
}

Assets::~Assets()
{

}

bool Assets::AddMesh(const std::string& filePath)
{
	// Fixes absolute paths
	std::string fileName = filePath.substr(filePath.find_last_of('/') + 1);
	fileName = fileName.substr(0, fileName.find_last_of('.')) + ".okayAsset";

	// Overrite old file / import new file
	Okay::VertexData data;
	VERIFY(Importer::Load(filePath, data));

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
	meshes.insert({ fileName, mesh });
	
	return true;
}

std::shared_ptr<Okay::Mesh> Assets::GetMesh(const std::string& fileName)
{
	// Need to change
	if (meshes.find(fileName) == meshes.end())
		return std::make_shared<Okay::Mesh>(); 

	return meshes[fileName];
}

bool Assets::AddTexture(const std::string& fileName)
{
	// Temp
	textures.insert({ fileName, std::make_shared<Okay::Texture>(fileName) });
}

std::shared_ptr<Okay::Texture> Assets::GetTexture(const std::string& fileName)
{
	// Need to change
	if (textures.find(fileName) == textures.end())
		return std::make_shared<Okay::Texture>(); 

	return textures[fileName];
}

bool Assets::LoadAll()
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
