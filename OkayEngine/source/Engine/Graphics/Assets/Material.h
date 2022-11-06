#pragma once
#include "Texture.h"
#include <memory>

namespace Okay
{
	class Material  
	{
	public:

		struct GPUData
		{
			Float2 uvTiling = { 1.f, 1.f };
			Float2 uvOffset = { 0.f, 0.f };
		};

		struct Description
		{
			std::string name;
			std::shared_ptr<Texture> baseColour;
			std::shared_ptr<Texture> specular;
			std::shared_ptr<Texture> ambient;
			GPUData gpuData;
			bool twoSided = false;
		};

		Material(const Description& desc);
		~Material();

		const std::string& getName() const;
		void setName(std::string_view name) { this->name = name; }

		void setGPUData(const GPUData& data);
		GPUData& getGPUData();
		const GPUData& getGPUData() const;

		Description getDesc() const;
		bool isValid() const;

		std::shared_ptr<const Texture> getBaseColour() const { return textures[0].lock(); } 
		std::shared_ptr<const Texture> getSpecular() const	 { return textures[1].lock(); } 
		std::shared_ptr<const Texture> getAmbient() const	 { return textures[2].lock(); }

		void setBaseColour(std::shared_ptr<const Texture> texture) { textures[0] = texture; }
		void setSpecular(std::shared_ptr<const Texture> texture)   { textures[1] = texture; }
		void setAmbient(std::shared_ptr<const Texture> texture)    { textures[2] = texture; }

	private:
		std::string name;
		std::weak_ptr<const Texture> textures[3];
		GPUData data;
		bool isTwoSided;

		void CheckValid(int index = -1) const;
	};
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