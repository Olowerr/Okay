#include "Material.h"
#include "Engine.h"

Okay::Material::Material()
	:isTwoSided(false)
{
	textures[0] = Okay::Engine::GetAssets().GetTexture("quack.jpg");
	textures[1] = Okay::Engine::GetAssets().GetTexture("quack.jpg");
	textures[2] = Okay::Engine::GetAssets().GetTexture("quack.jpg");
}

Okay::Material::Material(const MaterialDesc& desc)
{
	textures[0] = Okay::Engine::GetAssets().GetTexture(desc.baseColour.c_str);
	textures[1] = Okay::Engine::GetAssets().GetTexture(desc.specular.c_str);
	textures[2] = Okay::Engine::GetAssets().GetTexture(desc.ambient.c_str);

	data.uvTiling = desc.uvTiling;
	data.uvOffset = desc.uvOffset;
	isTwoSided = desc.twoSided;
}

Okay::Material::~Material()
{
}

void Okay::Material::BindTextures()
{
	static ID3D11ShaderResourceView* srvs[3]{};
	srvs[0] = *textures[0].get()->GetSRV();
	srvs[1] = *textures[1].get()->GetSRV();
	srvs[2] = *textures[2].get()->GetSRV();

	DX11::Get().GetDeviceContext()->PSSetShaderResources(0, 3, srvs);
}
