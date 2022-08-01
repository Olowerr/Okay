#pragma once
#include "Mesh.h"
#include "Material.h"

namespace Okay // Structs for now, change to classes
{
	enum struct Components
	{
		None = 0,
		Mesh,
		Transform,
		Tag
	};

#ifdef EDITOR
	struct BaseComp { };


	struct CompMesh : BaseComp
#else
	struct CompMesh 
#endif // EDITOR
	{
		static const Components ID = Components::Mesh;

		CompMesh();
		CompMesh(std::ifstream& reader);
		CompMesh(const std::string& meshName);

		void AssignMesh(const std::string& meshName);
		void AssignMaterial(UINT index, std::shared_ptr<Material> material);
		void AssignMaterial(UINT index, const Okay::String& materialName);
		std::shared_ptr<Material> GetMaterial() 
		{
			CheckMaterial();
			return std::shared_ptr<Material>(material);
		}

		std::shared_ptr<Mesh> mesh;
		std::weak_ptr<Material> material;

		void WritePrivateData(std::ofstream& writer);
		void ReadPrivateData(std::ifstream& reader);

		void CheckMaterial();
	};



#ifdef EDITOR
	struct CompTransform : BaseComp
#else
	struct CompTransform
#endif // EDITOR
	{
		static const Components ID = Components::Transform;

		CompTransform();
		CompTransform(Float3 pos, Float3 rot, Float3 scale);

		DirectX::XMFLOAT4X4 matrix;
		Float3 position;
		Float3 rotation;
		Float3 scale;

		void CalcMatrix();

		void WritePrivateData(std::ofstream& writer);
		void ReadPrivateData(std::ifstream& reader);
	};


#ifdef EDITOR
	struct CompTag : BaseComp
#else
	struct CompTag
#endif // EDITOR
	{
		static const Components ID = Components::Tag;

		CompTag() :tag("Entity") { }
		CompTag(const String& tag) :tag(tag) { }

		String tag;

		void WritePrivateData(std::ofstream& writer);
		void ReadPrivateData(std::ifstream& reader);
	};
}