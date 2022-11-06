#include "Material.h"

namespace Okay
{
	Material::Material(const Description& desc)
		:name(desc.name), data(desc.gpuData), isTwoSided(desc.twoSided)
	{
		textures[0] = desc.baseColour;
		textures[1] = desc.specular;
		textures[2] = desc.ambient;
	}

	Material::~Material()
	{
	}

	const std::string& Material::getName() const
	{
		return name;
	}

	void Material::setGPUData(const GPUData& data)
	{
		this->data = data;
	}

	Material::GPUData& Material::getGPUData()
	{
		return data;
	}

	const Material::GPUData& Material::getGPUData() const
	{
		return data;
	}

	Material::Description Material::getDesc() const
	{
		Description desc;
		desc.name = name;
		desc.baseColour = textures[0].lock();
		desc.specular = textures[1].lock();
		desc.ambient = textures[2].lock();
		desc.gpuData = data;

		return desc;
	}

	bool Material::isValid() const
	{
		return !textures[0].expired() && !textures[1].expired() && !textures[2].expired();
	}
}