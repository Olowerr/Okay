#include "ResourceManager.h"

Assets::Assets()
{

}

Assets::~Assets()
{

}

bool Assets::AddMesh(const std::string& file)
{
	Importer::VertexData data;
	VERIFY(Importer::Load(file, data));

	std::string fileName = file;
	fileName = fileName.substr(0, fileName.find_last_of('.')) + ".okayAsset";

	ReadDeclaration();

	files.emplace_back(fileName);
	WriteDeclaration();

	return true;
}

std::shared_ptr<Okay::Mesh> Assets::GetMesh(const std::string& fileName)
{
	auto& ptr = meshes[fileName];

	if (!ptr.get())
		ptr = std::make_shared<Okay::Mesh>();

	return ptr;
}

bool Assets::LoadAll()
{
		
	return true;
}

bool Assets::ReadDeclaration()
{
	std::ifstream reader(DeclarationPath.str, std::ios::binary);
	VERIFY(reader);

	UINT NumFiles = 0;
	reader.read((char*)&NumFiles, sizeof(UINT));
	VERIFY(NumFiles);

	files.resize(NumFiles);

	const UINT ByteWidth = sizeof(Okay::String) * NumFiles;
	reader.read((char*)files.data(), ByteWidth);

	reader.close();
	return true;
}

bool Assets::WriteDeclaration()
{
	std::ofstream writer(DeclarationPath.str, std::ios::binary);
	VERIFY(writer);

	const UINT NumFiles = (UINT)files.size();
	const UINT ByteWidth = sizeof(Okay::String) * NumFiles;

	writer.write((const char*)&NumFiles, sizeof(UINT));
	writer.write((const char*)files.data(), ByteWidth);

	writer.close();
	return true;
}
