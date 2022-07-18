#include "Material.h"

Okay::Material::Material()
	:isTwoSided(false)
{
	textures[0] = Engine::GetAssets().GetTexture("quack.jpg");
	textures[1] = Engine::GetAssets().GetTexture("quack.jpg");
	textures[2] = Engine::GetAssets().GetTexture("quack.jpg");
}

Okay::Material::Material(const MaterialDesc& desc)
{
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
