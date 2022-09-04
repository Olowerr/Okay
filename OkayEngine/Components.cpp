#include "Components.h"
#include "Engine.h"


namespace Okay
{

#pragma region
	/* ------ Mesh Component ------ */

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

	void CompMesh::AssignMesh(std::shared_ptr<Mesh> mesh)
	{
		this->mesh = mesh;
	}

	void CompMesh::AssignMaterial(UINT index, std::shared_ptr<Material> material)
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
	/* ------ Transform Component ------ */

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

	void CompTag::WritePrivateData(std::ofstream& writer)
	{
		writer.write(tag.c_str, sizeof(String));
	}

	void CompTag::ReadPrivateData(std::ifstream& reader)
	{
		reader.read(tag.c_str, sizeof(String));
	}
#pragma endregion Transform Component


#pragma region
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
	/* ------ Skeletal Mesh Component ------ */

	CompSkeletalMesh::CompSkeletalMesh()
		:aniTime(0.f), tickTime(0.f), currentFrame(0), playing(false)
	{
		mesh = Engine::GetAssets().GetSkeletalMesh("Default");
	}

	CompSkeletalMesh::CompSkeletalMesh(std::string_view meshName)
		:aniTime(0.f), tickTime(0.f), currentFrame(0), playing(false)
	{
		mesh = Engine::GetAssets().GetSkeletalMesh(meshName.data());
	}

	CompSkeletalMesh::CompSkeletalMesh(std::shared_ptr<SkeletalMesh> mesh)
		:aniTime(0.f), tickTime(0.f), currentFrame(0), playing(false)
	{
		this->mesh = mesh;
	}

	void CompSkeletalMesh::AssignMesh(std::string_view meshName)
	{
		//mesh = Engine::GetAssets().GetSkeletalMesh(meshName);
	}

	void CompSkeletalMesh::AssignMesh(std::shared_ptr<SkeletalMesh> mesh)
	{
		this->mesh = mesh;
	}

	void CompSkeletalMesh::AssignMaterial(UINT index, const String& materialName)
	{
		//material = Engine::GetAssets().GetMaterial(materialName);
	}

	void CompSkeletalMesh::AssignMaterial(UINT index, std::shared_ptr<Material> material)
	{
		this->material = material;
	}

	void CompSkeletalMesh::UpdateAnimation()
	{
		if (!playing)
			return;

		tickTime += Engine::GetDT();
		aniTime += Engine::GetDT();

		std::shared_ptr<Okay::SkeletalMesh> pMesh = mesh.lock();

		if (tickTime > pMesh->GetTickLengthS())
		{
			++currentFrame;
			tickTime = 0.f;
		}

		if (aniTime > pMesh->GetDurationS())
		{
			currentFrame = 0;
			aniTime = 0.f;
		}
	}

	void CompSkeletalMesh::StartAnimation()
	{
		aniTime = tickTime = 0.f;
		currentFrame = 0;
		playing = true;
	}

	void CompSkeletalMesh::StopAnimation()
	{
		aniTime = tickTime = 0.f;
		currentFrame = 0;
		playing = false;
	}

	void CompSkeletalMesh::UpdateSkeletalMatrices()
	{
		if (CheckMesh())
		{
			StopAnimation();
			return;
		}

		mesh.lock()->SetPose(currentFrame);
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

	bool CompSkeletalMesh::CheckMesh() const
	{
		if (mesh.expired())
		{
			mesh = Engine::GetAssets().GetSkeletalMesh("Default");
			return true;
		}

		return false;
	}

#pragma endregion Skeletal Mesh Component

}
