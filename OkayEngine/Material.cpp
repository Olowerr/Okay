#include "Material.h"
#include "Engine.h"

Okay::Material::Material()
	:isTwoSided(false), name("Default")
{
	textures[0] = Okay::Engine::GetAssets().GetTexture("quack.jpg");
	textures[1] = Okay::Engine::GetAssets().GetTexture("quack.jpg");
	textures[2] = Okay::Engine::GetAssets().GetTexture("quack.jpg");
}

Okay::Material::Material(const MaterialDesc_Strs& desc)
{
	textures[0] = Okay::Engine::GetAssets().GetTexture(desc.baseColour.c_str);
	textures[1] = Okay::Engine::GetAssets().GetTexture(desc.specular.c_str);
	textures[2] = Okay::Engine::GetAssets().GetTexture(desc.ambient.c_str);

	data.uvTiling = desc.uvTiling;
	data.uvOffset = desc.uvOffset;
	isTwoSided = desc.twoSided;
	name = desc.name;
}

Okay::Material::Material(const MaterialDesc_Ptrs& desc)
{
	textures[0] = desc.baseColour;
	textures[1] = desc.specular;
	textures[2] = desc.ambient;

	data.uvTiling = desc.uvTiling;
	data.uvOffset = desc.uvOffset;
	isTwoSided = desc.twoSided;
	name = desc.name;
}

Okay::Material::~Material()
{
}

void Okay::Material::BindTextures() const
{
	CheckValid();

	static ID3D11ShaderResourceView* srvs[3]{};
	srvs[0] = *textures[0].lock()->GetSRV();
	srvs[1] = *textures[1].lock()->GetSRV();
	srvs[2] = *textures[2].lock()->GetSRV();

	DX11::Get().GetDeviceContext()->PSSetShaderResources(0, 3, srvs);
}

const Okay::String& Okay::Material::GetName() const
{
	return name;
}

void Okay::Material::SetGPUData(Float2 uvTiling, Float2 uvOffset)
{
	data.uvTiling = uvTiling;
	data.uvOffset = uvOffset;
}

Okay::MaterialGPUData& Okay::Material::GetGPUData()
{
	return data;
}

const Okay::MaterialGPUData& Okay::Material::GetGPUData() const
{
	return data;
}

Okay::MaterialDesc_Strs Okay::Material::GetDesc() const
{
	CheckValid();

	Okay::MaterialDesc_Strs desc;
	desc.name = name;
	desc.baseColour = textures[0].lock()->GetName();
	desc.specular =	textures[1].lock()->GetName();
	desc.ambient = textures[2].lock()->GetName();
	desc.uvTiling = data.uvTiling;
	desc.uvOffset = data.uvOffset;

	return desc;
}

void Okay::Material::CheckValid(int index) const
{
	if (index != -1 && index < 3)
	{
		if (textures[index].expired())
			textures[index] = Engine::GetAssets().GetTexture("Default");

		return;
	}

	if (textures[0].expired())
		textures[0] = Engine::GetAssets().GetTexture("Default");
	
	if (textures[1].expired())
		textures[1] = Engine::GetAssets().GetTexture("Default");
	
	if (textures[2].expired())
		textures[2] = Engine::GetAssets().GetTexture("Default");
}
