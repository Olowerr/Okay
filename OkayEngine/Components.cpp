#include "Components.h"
#include "Engine.h"


/* ------ Mesh Component ------ */

Okay::CompMesh::CompMesh()
	:mesh(Engine::GetAssets().GetMesh("Default"))
{
	AssignMaterial(0, Engine::GetAssets().GetMaterial("Default"));
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

void Okay::CompMesh::AssignMesh(const std::shared_ptr<const Mesh>& mesh)
{
	this->mesh = mesh;
}

void Okay::CompMesh::AssignMaterial(UINT index, const std::shared_ptr<const Material>& material)
{
	// TEMP
	this->material = material;
}

void Okay::CompMesh::AssignMaterial(UINT index, const Okay::String& materialName)
{
	material = Engine::GetAssets().GetMaterial(materialName.c_str);
}

void Okay::CompMesh::WritePrivateData(std::ofstream& writer)
{
	CheckMaterial();
	CheckMesh();

	writer.write((const char*)&mesh.lock()->GetName(), sizeof(Okay::String));

	const UINT NumMaterials = 1;
	writer.write((const char*)&NumMaterials, sizeof(UINT));
	writer.write((const char*)&material.lock()->GetName(), sizeof(Okay::String));

#if 0
	const UINT NumMaterials = (UINT)materials.size();
	writer.write((const char*)&NumMaterials, sizeof(UINT));

	for (UINT i = 0; i < NumMaterials; i++)
	{
		std::shared_ptr<Material> currentMat(materials.at(i));
		writer.write((const char*)&currentMat->GetName(), sizeof(Okay::String));
	}
#endif
}

void Okay::CompMesh::ReadPrivateData(std::ifstream& reader)
{
	Okay::String readData;
	reader.read((char*)&readData, sizeof(Okay::String));
	mesh = Engine::GetAssets().GetMesh(readData.c_str);


	UINT NumMaterials = 0;
	reader.read((char*)&NumMaterials, sizeof(UINT));

	reader.read((char*)&readData, sizeof(Okay::String));
	material = Engine::GetAssets().GetMaterial(readData.c_str);

#if 0 
	UINT NumMaterials = 0;

	materials.resize(NumMaterials);

	for (UINT i = 0; i < NumMaterials; i++)
	{
		reader.read((char*)&readData, sizeof(Okay::String));

		materials.at(i) = Engine::GetAssets().GetMaterial(readData.c_str);
	}
#endif
}

void Okay::CompMesh::CheckMaterial() const
{
	if (!material.expired())
		return;

	material = Engine::GetAssets().GetMaterial("Default");
}

void Okay::CompMesh::CheckMesh() const
{
	if (!mesh.expired())
		return;

	mesh = Engine::GetAssets().GetMesh("Default");
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


/* ------ Tag Component ------ */

void Okay::CompTag::WritePrivateData(std::ofstream& writer)
{
	writer.write(tag.c_str, sizeof(Okay::String));
}

void Okay::CompTag::ReadPrivateData(std::ifstream& reader)
{
	reader.read(tag.c_str, sizeof(Okay::String));
}
