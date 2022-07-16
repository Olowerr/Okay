#include "ResourceManager.h"

Assets::Assets()
{
	LoadAll();
}

Assets::~Assets()
{

}

bool Assets::AddMesh(const std::string& filePath)
{
	ReadDeclaration();

	// Fixes absolute paths
	std::string fileName = filePath.substr(filePath.find_last_of('/') + 1);
	fileName = fileName.substr(0, fileName.find_last_of('.')) + ".okayAsset";

	for (auto& file : files)
	{
		if (file == fileName)
			return true;
	}
	
	// Attempt to load .obj / .fbx
	Okay::VertexData data;
	VERIFY(Importer::Load(filePath, data));

	std::shared_ptr<Okay::Mesh> mesh = std::make_shared<Okay::Mesh>(data, fileName);
	meshes.insert({ fileName, mesh });

	// Add the asset to the declaration
	files.emplace_back(fileName);
	WriteDeclaration();

	return true;
}

std::shared_ptr<Okay::Mesh> Assets::GetMesh(const std::string& fileName)
{
	if (meshes.find(fileName) == meshes.end())
		return std::make_shared<Okay::Mesh>(); // Returns the default mesh

	return meshes[fileName];
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
