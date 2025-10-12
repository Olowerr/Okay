#include "Material.h"

namespace Okay
{
	Material::Material()
		:name("Material"), isTwoSided(false)
	{
		textures[0] = 0u;
		textures[1] = 0u;
		textures[2] = 0u;
	}

	Material::Material(const Description& desc)
		:name(desc.name), data(desc.gpuData), isTwoSided(desc.twoSided)
	{
		textures[BASECOLOUR_INDEX] = desc.textureIDs[BASECOLOUR_INDEX];
		textures[SPECULAR_INDEX] = desc.textureIDs[SPECULAR_INDEX];
		textures[METALLIC_INDEX] = desc.textureIDs[METALLIC_INDEX];
	}

	Material::~Material()
	{
	}

	Material::Material(Material&& other) noexcept
		:name(std::move(other.name)), data(other.data), isTwoSided(other.isTwoSided)
	{
		textures[0] = other.textures[0];
		other.textures[0] = 0u;
		textures[1] = other.textures[1];
		other.textures[1] = 0u;
		textures[2] = other.textures[2];
		other.textures[2] = 0u;

		other.isTwoSided = false;
		other.data = GPUData{};
	}

	Material::Description Material::getDesc() const
	{
		Description desc;
		desc.name = name;
		desc.textureIDs[0] = textures[0];
		desc.textureIDs[1] = textures[1];
		desc.textureIDs[2] = textures[2];
		desc.gpuData = data;
		desc.twoSided = isTwoSided;

		return desc;
	}
}