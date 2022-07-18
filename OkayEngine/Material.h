#pragma once
#include "Engine.h"
 
namespace Okay
{
	struct MaterialDesc
	{
		String baseColourTexture;
		String specularTexture;
		String ambientTexture;
		Float2 uvTiling;
		Float2 uvOffset;
		bool twoSided;
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