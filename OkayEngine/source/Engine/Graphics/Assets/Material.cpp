#include "Material.h"

namespace Okay
{
	Material::Material()
		:name("Material"), data(), isTwoSided(false)
	{
		textures[0] = 0u;
		textures[1] = 0u;
		textures[2] = 0u;
	}

	Material::Material(const Description& desc)
		:name(desc.name), data(desc.gpuData), isTwoSided(desc.twoSided)
	{
		textures[0] = desc.baseColourTexIndex;
		textures[1] = desc.specularTexIndex;
		textures[2] = desc.ambientTexIndex;
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
		desc.baseColourTexIndex = textures[0];
		desc.specularTexIndex = textures[1];
		desc.ambientTexIndex = textures[2];
		desc.gpuData = data;
		desc.twoSided = isTwoSided;

		return desc;
	}

	//bool Material::isValid() const
	//{
	//	return !textures[0].expired() && !textures[1].expired() && !textures[2].expired();
	//}
}