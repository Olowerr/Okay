#include "Components.h"
#include "Engine.h"


/* ------ Mesh Component ------ */

Okay::CompMesh::CompMesh()
	:mesh(Engine::GetAssets().GetMesh("Default"))
{
}

Okay::CompMesh::CompMesh(const std::string& meshName)
	:mesh(Engine::GetAssets().GetMesh(meshName))
{
}

void Okay::CompMesh::AssignMesh(const std::string& meshName)
{
	mesh = Engine::GetAssets().GetMesh(meshName);
}

void Okay::CompMesh::AssignMaterial(UINT index, std::shared_ptr<Material> material)
{
	// if (mesh->NumSub >= index)
	//	return;

	// materials.clear(); // Drop references to old materials // Only resize might be fine idk
	// materials.resize(mesh->NumSub);

	// TEMP
	materials.resize(1);

	materials.at(index) = material.get();
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