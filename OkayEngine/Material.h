#pragma once
#include "Texture.h"
 
namespace Okay
{
	struct MaterialDesc_Strs
	{
		String baseColour;
		String specular;
		String ambient;
		Float2 uvTiling = { 1.f, 1.f };
		Float2 uvOffset = { 0.f, 0.f };
		bool twoSided = false;
	};
	
	struct MaterialDesc_Ptrs
	{
		std::shared_ptr<Texture> baseColour;
		std::shared_ptr<Texture> specular;
		std::shared_ptr<Texture> ambient;
		Float2 uvTiling = { 1.f, 1.f };
		Float2 uvOffset = { 0.f, 0.f };
		bool twoSided = false;
	};

	class Material  
	{
	public:
		Material();
		Material(const MaterialDesc_Strs& desc);
		Material(const MaterialDesc_Ptrs& desc);
		~Material();

		void BindTextures() const;


		struct GPUData 
		{
			Float2 uvTiling = {1.f, 1.f};
			Float2 uvOffset = {0.f, 0.f};
		};

		void SetGPUData(Float2 uvTiling, Float2 uvOffset);
		const GPUData& GetGPUData() const;

	private:
		std::shared_ptr<Texture> textures[3];
		GPUData data;
		bool isTwoSided;

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