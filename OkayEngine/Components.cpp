#include "Components.h"
#include "Engine.h"


/* ------ Mesh Component ------ */

Okay::CompMesh::CompMesh()
	:mesh(Engine::GetAssets().GetMesh("Default"))
{
	AssignMaterial(0, Engine::GetAssets().GetMaterial("Default"));
}

Okay::CompMesh::CompMesh(std::ifstream& reader)
{
	ReadPrivateData(reader);
}

Okay::CompMesh::CompMesh(const std::string& meshName)
	:mesh(Engine::GetAssets().GetMesh(meshName))
{
	//materials.clear(); 
	//materials.resize(mesh->NumSub);

	//for (size_t i = 0; i < mesh->NumSub; i++)
	AssignMaterial(0, Engine::GetAssets().GetMaterial("Default"));
}

void Okay::CompMesh::AssignMesh(const std::string& meshName)
{
	mesh = Engine::GetAssets().GetMesh(meshName);
	//materials.clear();
	//materials.resize(mesh->NumSub);
}

void Okay::CompMesh::AssignMaterial(UINT index, std::shared_ptr<Material> material)
{
	// if (mesh->NumSub >= index)
	//	return;

	// materials.clear(); // Drop references to old materials // Only resize might be fine idk
	// materials.resize(mesh->NumSub);

	// TEMP
	materials.resize(1);
	materials.at(0) = material.get();
}

void Okay::CompMesh::WritePrivateData(std::ofstream& writer)
{
	writer.write((const char*)&mesh->GetName(), sizeof(Okay::String));

	const UINT NumMaterials = (UINT)materials.size();
	writer.write((const char*)&NumMaterials, sizeof(UINT));

	for (UINT i = 0; i < NumMaterials; i++)
		writer.write((const char*)&materials.at(i)->GetName(), sizeof(Okay::String));
}

void Okay::CompMesh::ReadPrivateData(std::ifstream& reader)
{
	Okay::String readData;
	reader.read((char*)&readData, sizeof(Okay::String));
	mesh = Engine::GetAssets().GetMesh(readData.c_str);

	UINT NumMaterials = 0;
	reader.read((char*)&NumMaterials, sizeof(UINT));

	materials.resize(NumMaterials);

	for (UINT i = 0; i < NumMaterials; i++)
	{
		reader.read((char*)&readData, sizeof(Okay::String));

		auto asd = Engine::GetAssets().GetMaterial(readData.c_str);
		materials.at(i) = asd.get();
	}

	
}


/* ------ Transform Component ------ */

Okay::CompTransform::CompTransform()
	:position(), rotation(), scale(1.f, 1.f, 1.f)
{
	CalcMatrix();
}

Okay::CompTransform::CompTransform(Float3 pos, Float3 rot, Float3 scale)
	:position(pos), rotation(rot), scale(scale)
{
	CalcMatrix();
}

void Okay::CompTransform::CalcMatrix()
{
	using namespace DirectX;
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(
		XMMatrixScaling(scale.x, scale.y, scale.z) *
		XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
		XMMatrixTranslation(position.x, position.y, position.z)
	));
}

void Okay::CompTransform::WritePrivateData(std::ofstream& writer)
{
	writer.write((const char*)&position, sizeof(Okay::Float3));
	writer.write((const char*)&rotation, sizeof(Okay::Float3));
	writer.write((const char*)&scale, sizeof(Okay::Float3));
}

void Okay::CompTransform::ReadPrivateData(std::ifstream& reader)
{
	reader.read((char*)&position, sizeof(Okay::Float3));
	reader.read((char*)&rotation, sizeof(Okay::Float3));
	reader.read((char*)&scale, sizeof(Okay::Float3));

	CalcMatrix();
}
