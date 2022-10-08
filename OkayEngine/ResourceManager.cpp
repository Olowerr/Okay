#include "ResourceManager.h"
#include "Engine.h"

Assets::Assets()
{
	// Make sure Okay::Engine::Get() is never called here
	printf("Assets Constructor\n");
}

Assets::~Assets()
{

}

void Assets::SetUp()
{
	AddTexture(TexturePath + "quack.jpg");
	std::shared_ptr<Okay::Texture> defaultTexture = GetTexture("quack.jpg");

	Okay::MaterialDesc_Ptrs defaultDesc;
	defaultDesc.name = "Default";
	defaultDesc.baseColour = defaultTexture;
	defaultDesc.specular = defaultTexture;
	defaultDesc.ambient = defaultTexture;

	AddMaterial(defaultDesc);
	LoadDeclared();
	
	// Manually modify assets here -----
	
	LoadSkeletalMesh("../content/meshes/ani/gobWalk3.fbx");

	// -----

	WriteDeclaration();
	ClearDeclared();
}

void Assets::Save()
{
	WriteDeclaration();
}

bool Assets::TryImport(std::string_view path)
{
	const std::string_view fileEnding = path.substr(path.find_last_of('.'));
	bool result = false;

	if (Okay::Texture::IsValid(path))
		result = AddTexture(path.data());

	else if (fileEnding == ".fbx" || fileEnding == ".FBX" || fileEnding == ".obj" || fileEnding == ".OBJ")
		result = AddMesh(path.data());

	VERIFY(result);

	WriteDeclaration();
	ClearDeclared();

	return true;
}

bool Assets::AddMesh(const std::string& filePath)
{
	// Fixes absolute paths

	size_t pos = filePath.find_last_of('/');
	pos = pos == -1 ? filePath.find_last_of('\\') : pos;


	std::string fileLocation = filePath.substr(0, pos + 1);

	// Overrite old file / import new file
	Okay::VertexData data;
	std::string output[4];
	VERIFY(Importer::Load(filePath, data, output));

	if (output[1].size())
		AddTexture(fileLocation + output[1]);
	if (output[2].size())
		AddTexture(fileLocation + output[2]);
	if (output[3].size())
		AddTexture(fileLocation + output[3]);

	if (!MaterialExists(output[0]))
	{
		Okay::MaterialDesc_Ptrs matDesc;
		matDesc.name = output[0];
		matDesc.baseColour = GetTexture(output[1]);
		matDesc.specular = GetTexture(output[2]);
		matDesc.ambient = GetTexture(output[3]);
		
		AddMaterial(matDesc);
	}

	std::string fileName = filePath.substr(pos + 1);
	fileName = fileName.substr(0, fileName.find_last_of('.'));

	// Create the mesh
	meshes[fileName] = std::make_shared<Okay::Mesh>(data, fileName);

	return true;
}

bool Assets::MeshExists(const std::string& fileName)
{
	return meshes.find(fileName) != meshes.end();
}

std::shared_ptr<Okay::SkeletalMesh> Assets::GetSkeletalMesh(const std::string& name)
{
	if (skeletalMeshes.find(name) == skeletalMeshes.end())
	{
		skeletalMeshes.insert({ "Default", std::make_shared<Okay::SkeletalMesh>() });
		return skeletalMeshes["Default"];
	}

	return skeletalMeshes[name];
}

std::shared_ptr<Okay::Mesh> Assets::GetMesh(const std::string& fileName)
{
	if (!MeshExists(fileName))
	{
		meshes.insert({ "Default", std::make_shared<Okay::Mesh>() });
		return meshes["Default"];
	}

	return meshes[fileName];
}

const Okay::String& Assets::GetMeshName(UINT index)
{
	auto it = meshes.begin();
	std::advance(it, index);
	return it->second.get()->GetName();
}

void Assets::ChangeMeshName(std::weak_ptr<Okay::Mesh> mesh, const Okay::String& name)
{
	if (mesh.expired())
		return;

	const Okay::String& oldName = mesh.lock()->GetName();
	
	if (!MeshExists(oldName.c_str))
		return;

	// Rename file
	const std::string oldPath = "../Content/Meshes/" + std::string(oldName) + ".OkayAsset";
	const std::string newPath = "../Content/Meshes/" + std::string(name) + ".OkayAsset";

	if (rename(oldPath.c_str(), newPath.c_str()) != 0)
		return;

	auto node = meshes.extract(oldName.c_str);
	node.key() = name;
	node.mapped()->SetName(name);

	meshes.insert(std::move(node));
}

void Assets::RemoveMesh(std::weak_ptr<Okay::Mesh> mesh)
{
	if (mesh.expired())
		return;

	meshes.erase(mesh.lock()->GetName().c_str);
}

bool Assets::AddTexture(const std::string& filePath)
{
	size_t pos = filePath.find_last_of('/');
	pos = pos == -1 ? filePath.find_last_of('\\') : pos;

	const std::string& fileName = filePath.substr(pos + 1);

	if (textures.find(fileName) != textures.end() || !Okay::Texture::IsValid(filePath))
		return false;

	textures.insert({ fileName, std::make_shared<Okay::Texture>(filePath) });

	// TEMP path
	std::string savePath = TexturePath + fileName;

	CopyFile(std::wstring(filePath.begin(), filePath.end()).c_str(), std::wstring(savePath.begin(), savePath.end()).c_str(), TRUE);

	return true;
}

bool Assets::TextureExists(const std::string& fileName)
{
	return textures.find(fileName) != textures.end();
}

std::shared_ptr<Okay::Texture> Assets::GetTexture(const std::string& fileName)
{
	// Need to change
	if (textures.find(fileName) == textures.end())
		return textures["quack.jpg"];

	return textures[fileName];
}

const Okay::String& Assets::GetTextureName(UINT index)
{
	auto it = textures.begin();
	std::advance(it, index);
	return it->second.get()->GetName();
}

void Assets::ChangeTextureName(std::weak_ptr<Okay::Texture> texture, const Okay::String& name)
{
	if (texture.expired())
		return;

	const Okay::String& oldName = texture.lock()->GetName();

	if (!TextureExists(oldName.c_str))
		return;

	auto node = textures.extract(oldName.c_str);
	node.key() = name;
	node.mapped()->SetName(name);

	textures.insert(std::move(node));
}

void Assets::RemoveTexture(std::weak_ptr<Okay::Texture> texture)
{
	if (texture.expired())
		return;

	textures.erase(texture.lock()->GetName().c_str);
}

bool Assets::AddMaterial(const Okay::MaterialDesc_Strs& matDesc)
{
	if (MaterialExists(matDesc.name.c_str))
		return false;

	materials.insert({ matDesc.name.c_str, std::make_shared<Okay::Material>(matDesc) });
	return true;
}

bool Assets::AddMaterial(const Okay::MaterialDesc_Ptrs& matDesc)
{
	if (MaterialExists(matDesc.name.c_str))
		return false;

	materials.insert({ matDesc.name.c_str, std::make_shared<Okay::Material>(matDesc) });
	return true;
}

bool Assets::MaterialExists(const std::string& matName)
{
	return materials.find(matName) != materials.end();
}

void Assets::ChangeMaterialName(std::weak_ptr<Okay::Material> material, const Okay::String& name)
{
	if (material.expired())
		return;

	const Okay::String& oldName = material.lock()->GetName();

	if (!MaterialExists(oldName.c_str))
		return;

	auto node = materials.extract(oldName.c_str);
	node.key() = name;
	node.mapped()->SetName(name);

	materials.insert(std::move(node));
}

void Assets::RemoveMaterial(std::weak_ptr<Okay::Material> material)
{
	if (material.expired())
		return;

	materials.erase(material.lock()->GetName().c_str);
}

std::shared_ptr<Okay::Material> Assets::GetMaterial(const std::string& materialName)
{
	if (materials.find(materialName) == materials.end())
		return std::make_shared<Okay::Material>();

	return materials[materialName];
}

std::shared_ptr<Okay::Material> Assets::GetMaterial(UINT index)
{
	auto it = materials.begin();
	std::advance(it, index);
	return it->second;
}

const Okay::String& Assets::GetMaterialName(UINT index)
{
	auto it = materials.begin();
	std::advance(it, index);
	return it->second.get()->GetName();
}

bool Assets::LoadDeclared()
{
	VERIFY(ReadDeclaration());

	std::string file;

	for (const Okay::String& mesh : decMeshes)
	{
		file = mesh;
		file = file.substr(0, file.find_last_of('.'));
		//file += ".OkayAsset";

		// Attempt to load the file
		Okay::VertexData data;
		if (!Importer::LoadOkayAsset(file, data))
			continue;

		// Create mesh and insert into map
		meshes[file] = std::make_shared<Okay::Mesh>(data, file);
	}

	for (const Okay::String& texture : decTextures)
	{
		if (!Okay::Texture::IsValid(TexturePath + texture.c_str))
			continue;

		textures[texture.c_str] = std::make_shared<Okay::Texture>(TexturePath + texture.c_str);
	}

	for (const Okay::MaterialDesc_Strs& materialDesc : decMaterials)
	{
		materials[materialDesc.name.c_str] = std::make_shared<Okay::Material>(materialDesc);
	}

	return true;
}

void Assets::ClearDeclared()
{
	decMeshes.resize(0);
	decTextures.resize(0);
	decMaterials.resize(0);
}

bool Assets::ReadDeclaration()
{
	std::ifstream reader(DeclarationPath, std::ios::binary);
	VERIFY(reader);

	UINT numElements = 0;
	UINT byteWidth = 0;

	// Meshes
	reader.read((char*)&numElements, sizeof(UINT));
	decMeshes.resize(numElements);
	reader.read((char*)decMeshes.data(), sizeof(Okay::String) * numElements);

	// Textures
	reader.read((char*)&numElements, sizeof(UINT));
	decTextures.resize(numElements);
	reader.read((char*)decTextures.data(), sizeof(Okay::String) * numElements);

	// Materials
	reader.read((char*)&numElements, sizeof(UINT));
	decMaterials.resize(numElements);
	reader.read((char*)decMaterials.data(), sizeof(Okay::MaterialDesc_Strs) * numElements);


	reader.close();
	return true;
}

bool Assets::WriteDeclaration()
{
	std::ofstream writer(DeclarationPath, std::ios::binary | std::ios::trunc);
	VERIFY(writer);

	UINT numElements = 0;
	UINT byteWidth = 0;

	// Meshes
	UINT c = 0;
	decMeshes.resize(meshes.size());
	for (auto& mesh : meshes)
		decMeshes.at(c++) = mesh.second->GetName();

	// Textures
	c = 0;
	decTextures.resize(textures.size());
	for (auto& tex : textures)
		decTextures.at(c++) = tex.second->GetName();

	// Materials
	c = 0;
	decMaterials.resize(materials.size());
	for (auto& mat : materials)
		decMaterials.at(c++) = mat.second->GetDesc();



	// Meshes
	numElements = (UINT)decMeshes.size();
	byteWidth = sizeof(Okay::String) * numElements;
	writer.write((const char*)&numElements, sizeof(UINT));
	writer.write((const char*)decMeshes.data(), byteWidth);

	// Textures
	numElements = (UINT)decTextures.size();
	byteWidth = sizeof(Okay::String) * numElements;
	writer.write((const char*)&numElements, sizeof(UINT));
	writer.write((const char*)decTextures.data(), byteWidth);

	// Materials
	numElements = (UINT)decMaterials.size();
	byteWidth = sizeof(Okay::MaterialDesc_Strs) * numElements;
	writer.write((const char*)&numElements, sizeof(UINT));
	writer.write((const char*)decMaterials.data(), byteWidth);

	writer.close();
	return true;
}
