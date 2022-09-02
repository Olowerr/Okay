#include "Components.h"
#include "Engine.h"


namespace Okay
{

#pragma region
	CompMesh::CompMesh()
		:mesh(Engine::GetAssets().GetMesh("Default"))
	{
		AssignMaterial(0, Engine::GetAssets().GetMaterial("Default"));
	}

	CompMesh::CompMesh(const std::string& meshName)
		:mesh(Engine::GetAssets().GetMesh(meshName))
	{
		//materials.clear(); 
		//materials.resize(mesh->NumSub);

		//for (size_t i = 0; i < mesh->NumSub; i++)
		AssignMaterial(0, Engine::GetAssets().GetMaterial("Default"));
	}

	void CompMesh::AssignMesh(const std::string& meshName)
	{
		mesh = Engine::GetAssets().GetMesh(meshName);
		//materials.clear();
		//materials.resize(mesh->NumSub);
	}

	void CompMesh::AssignMesh(const std::shared_ptr<const Mesh>& mesh)
	{
		this->mesh = mesh;
	}

	void CompMesh::AssignMaterial(UINT index, const std::shared_ptr<const Material>& material)
	{
		// TEMP
		this->material = material;
	}

	void CompMesh::AssignMaterial(UINT index, const String& materialName)
	{
		material = Engine::GetAssets().GetMaterial(materialName.c_str);
	}

	void CompMesh::WritePrivateData(std::ofstream& writer)
	{
		CheckMaterial();
		CheckMesh();

		writer.write((const char*)&mesh.lock()->GetName(), sizeof(String));

		const UINT NumMaterials = 1;
		writer.write((const char*)&NumMaterials, sizeof(UINT));
		writer.write((const char*)&material.lock()->GetName(), sizeof(String));

#if 0
		const UINT NumMaterials = (UINT)materials.size();
		writer.write((const char*)&NumMaterials, sizeof(UINT));

		for (UINT i = 0; i < NumMaterials; i++)
		{
			std::shared_ptr<Material> currentMat(materials.at(i));
			writer.write((const char*)&currentMat->GetName(), sizeof(String));
		}
#endif
	}

	void CompMesh::ReadPrivateData(std::ifstream& reader)
	{
		String readData;
		reader.read((char*)&readData, sizeof(String));
		mesh = Engine::GetAssets().GetMesh(readData.c_str);


		UINT NumMaterials = 0;
		reader.read((char*)&NumMaterials, sizeof(UINT));

		reader.read((char*)&readData, sizeof(String));
		material = Engine::GetAssets().GetMaterial(readData.c_str);

#if 0 
		UINT NumMaterials = 0;

		materials.resize(NumMaterials);

		for (UINT i = 0; i < NumMaterials; i++)
		{
			reader.read((char*)&readData, sizeof(String));

			materials.at(i) = Engine::GetAssets().GetMaterial(readData.c_str);
		}
#endif
	}

	void CompMesh::CheckMaterial() const
	{
		if (!material.expired())
			return;

		material = Engine::GetAssets().GetMaterial("Default");
	}

	void CompMesh::CheckMesh() const
	{
		if (!mesh.expired())
			return;

		mesh = Engine::GetAssets().GetMesh("Default");
	}
#pragma endregion Mesh Component


#pragma region

	CompTransform::CompTransform()
		:position(), rotation(), scale(1.f, 1.f, 1.f)
	{
		CalcMatrix();
	}

	CompTransform::CompTransform(Float3 pos, Float3 rot, Float3 scale)
		:position(pos), rotation(rot), scale(scale)
	{
		CalcMatrix();
	}

	void CompTransform::CalcMatrix()
	{
		using namespace DirectX;
		auto t = XMMatrixTranspose(
			XMMatrixScaling(scale.x, scale.y, scale.z) *
			XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
			XMMatrixTranslation(position.x, position.y, position.z)
		);
		XMStoreFloat4x4(&matrix, t);
	}

	void CompTransform::WritePrivateData(std::ofstream& writer)
	{
		writer.write((const char*)&position, sizeof(Float3));
		writer.write((const char*)&rotation, sizeof(Float3));
		writer.write((const char*)&scale, sizeof(Float3));
	}

	void CompTransform::ReadPrivateData(std::ifstream& reader)
	{
		reader.read((char*)&position, sizeof(Float3));
		reader.read((char*)&rotation, sizeof(Float3));
		reader.read((char*)&scale, sizeof(Float3));

		CalcMatrix();
	}
#pragma endregion Transform Component


#pragma region

	void CompTag::WritePrivateData(std::ofstream& writer)
	{
		writer.write(tag.c_str, sizeof(String));
	}

	void CompTag::ReadPrivateData(std::ifstream& reader)
	{
		reader.read(tag.c_str, sizeof(String));
	}


	/* ------ Point Light Component ------ */

	void CompPointLight::WritePrivateData(std::ofstream& writer)
	{
		writer.write((const char*)this, sizeof(CompPointLight));
	}

	void CompPointLight::ReadPrivateData(std::ifstream& reader)
	{
		reader.read((char*)this, sizeof(CompPointLight));
	}
#pragma endregion Tag Component


#pragma region
	CompSkeletalMesh::CompSkeletalMesh(const std::string& meshName)
	{
		//mesh = Engine::GetAssets().GetSkeletalMesh(meshName);
	}

	CompSkeletalMesh::CompSkeletalMesh(const std::shared_ptr<const SkeletalMesh>& mesh)
	{
		this->mesh = mesh;
	}

	void CompSkeletalMesh::AssignMesh(const std::string& meshName)
	{
		//mesh = Engine::GetAssets().GetSkeletalMesh(meshName);
	}

	void CompSkeletalMesh::AssignMesh(const std::shared_ptr<const SkeletalMesh>& mesh)
	{
		this->mesh = mesh;
	}

	void CompSkeletalMesh::AssignMaterial(UINT index, const String& materialName)
	{
		//material = Engine::GetAssets().GetMaterial(materialName);
	}

	void CompSkeletalMesh::AssignMaterial(UINT index, const std::shared_ptr<const Material>& material)
	{
		this->material = material;
	}

	void CompSkeletalMesh::WritePrivateData(std::ofstream& writer)
	{
	}

	void CompSkeletalMesh::ReadPrivateData(std::ifstream& reader)
	{
	}

	void CompSkeletalMesh::CheckMaterial() const
	{
		if (material.expired())
			material = Engine::GetAssets().GetMaterial("Default");
	}

	void CompSkeletalMesh::CheckMesh() const
	{
		
	}

#pragma endregion Skeletal Mesh Component

}
