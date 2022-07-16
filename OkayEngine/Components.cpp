#include "Components.h"
#include "Engine.h"


/* ------ Mesh Component ------ */

Okay::CompMesh::CompMesh()
	:mesh(Engine::GetAssets().GetMesh("Default"))
{
}

Okay::CompMesh::CompMesh(const Okay::String& meshName)
	:mesh(Engine::GetAssets().GetMesh(meshName.c_str))
{
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