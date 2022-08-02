#pragma once
#include "Texture.h"
 
namespace Okay
{
	struct MaterialDesc_Strs
	{
		String name;
		String baseColour;
		String specular;
		String ambient;
		Float2 uvTiling = { 1.f, 1.f };
		Float2 uvOffset = { 0.f, 0.f };
		bool twoSided = false;
	};
	
	struct MaterialDesc_Ptrs
	{
		String name;
		std::shared_ptr<Texture> baseColour;
		std::shared_ptr<Texture> specular;
		std::shared_ptr<Texture> ambient;
		Float2 uvTiling = { 1.f, 1.f };
		Float2 uvOffset = { 0.f, 0.f };
		bool twoSided = false;
	};

	struct MaterialGPUData 
	{
		Float2 uvTiling = {1.f, 1.f};
		Float2 uvOffset = {0.f, 0.f};
	};

	class Material  
	{
	public:
		Material();
		Material(const MaterialDesc_Strs& desc);
		Material(const MaterialDesc_Ptrs& desc);
		~Material();

		void BindTextures() const;
		const Okay::String& GetName() const;
		void SetName(const Okay::String& name) { this->name = name; }

		void SetGPUData(Float2 uvTiling, Float2 uvOffset);
		MaterialGPUData& GetGPUData();

		MaterialDesc_Strs GetDesc() const;

		std::shared_ptr<Texture> GetBaseColour() { return textures[0].lock(); }
		std::shared_ptr<Texture> GetSpecular() { return textures[1].lock(); }
		std::shared_ptr<Texture> GetAmbient() { return textures[2].lock(); }

	private:
		Okay::String name;
		mutable std::weak_ptr<Texture> textures[3];
		MaterialGPUData data;
		bool isTwoSided;

		void CheckValid() const;

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