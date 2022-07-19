#pragma once
#include "Texture.h"
 
namespace Okay
{
	struct MaterialDesc
	{
		String baseColour;
		String specular;
		String ambient;
		Float2 uvTiling = { 1.f, 1.f };
		Float2 uvOffset = { 0.f, 0.f };
		bool twoSided = false;
	};
	
	class Material  
	{
	public:
		Material();
		Material(const MaterialDesc& desc);
		~Material();

		void BindTextures();

		struct GPUData 
		{
			Float2 uvTiling = {1.f, 1.f};
			Float2 uvOffset = {0.f, 0.f};
		};

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