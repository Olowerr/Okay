#pragma once
#include "Engine/Okay/Okay.h"
#include "glm/glm.hpp"

namespace Okay
{
	class Material  
	{
	public:

		static const uint32_t BASECOLOUR_INDEX = 0u;
		static const uint32_t SPECULAR_INDEX = 1u;
		static const uint32_t AMBIENT_INDEX = 2u; // Rmv ambient?

		struct GPUData
		{
			glm::vec2 uvTiling = { 1.f, 1.f };
			glm::vec2 uvOffset = { 0.f, 0.f };
			float shinynessExp = 50.f;
			float padding[3];
		};

		struct Description
		{
			Description() = default;
			Description(std::string_view name, uint32_t baseIdx, uint32_t specIdx, uint32_t ambiIdx, GPUData gpuData, bool twoSided = false)
				:name(name), baseColourTexIndex(baseIdx), specularTexIndex(specIdx), ambientTexIndex(ambiIdx), gpuData(gpuData), twoSided(twoSided)
			{ }

			std::string_view name;
			uint32_t baseColourTexIndex = 0u;
			uint32_t specularTexIndex = 0u;
			uint32_t ambientTexIndex = 0u;
			GPUData gpuData;
			bool twoSided = false;
		};

		Material();
		Material(const Description& desc);
		Material(Material&& other) noexcept;
		~Material();

		inline const std::string& getName() const;
		inline void setName(std::string_view name);

		inline void setGPUData(const GPUData& data);
		inline GPUData& getGPUData();
		inline const GPUData& getGPUData() const;

		Description getDesc() const;
		//bool isValid() const;

		inline uint32_t getBaseColour() const;
		inline uint32_t getSpecular() const	;
		inline uint32_t getAmbient() const;

		inline void setBaseColour(uint32_t textureIdx);
		inline void setSpecular(uint32_t textureIdx);
		inline void setAmbient(uint32_t textureIdx);

	private:
		std::string name;
		uint32_t textures[3];
		GPUData data;
		bool isTwoSided;
	};


	inline void Material::setName(std::string_view name) { this->name = name; }
	inline const std::string& Material::getName() const  { return name; }

	inline void Material::setGPUData(const Material::GPUData& data) { this->data = data; }
	inline Material::GPUData& Material::getGPUData()				{ return data; }
	inline const Material::GPUData& Material::getGPUData() const	{ return data; }

	inline void Material::setBaseColour(uint32_t textureIdx) { textures[0] = textureIdx; }
	inline void Material::setSpecular(uint32_t textureIdx)	 { textures[1] = textureIdx; }
	inline void Material::setAmbient(uint32_t textureIdx)	 { textures[2] = textureIdx; }

	inline uint32_t Material::getBaseColour() const { return textures[0]; }
	inline uint32_t Material::getSpecular() const	{ return textures[1]; }
	inline uint32_t Material::getAmbient() const	{ return textures[2]; }
}


/*

	BaseColour
	Specular
	Ambient
	Tiling
	Offset
	Two sided

	Normal
	Emission
	Tessellation
	Opacity
*/