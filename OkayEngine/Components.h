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

	struct CompMesh 
	{
		static const Components ID = Components::Mesh;

		CompMesh();
		CompMesh(std::ifstream& reader);
		CompMesh(const std::string& meshName);

		void AssignMesh(const std::string& meshName);
		void AssignMesh(const std::shared_ptr<const Mesh>& mesh);

		void AssignMaterial(UINT index, const Okay::String& materialName);
		void AssignMaterial(UINT index, const std::shared_ptr<const Material>& material);

		std::shared_ptr<const Material> GetMaterial() const
		{
			CheckMaterial();
			return std::shared_ptr<const Material>(material);
		}
		std::shared_ptr<const Mesh> GetMesh() const
		{
			CheckMesh();
			return std::shared_ptr<const Mesh>(mesh);
		}

		mutable std::weak_ptr<const Mesh> mesh;
		mutable std::weak_ptr<const Material> material;

		void WritePrivateData(std::ofstream& writer);
		void ReadPrivateData(std::ifstream& reader);

		void CheckMaterial() const;
		void CheckMesh() const;
	};

	struct CompTransform
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

	struct CompTag
	{
		static const Components ID = Components::Tag;

		CompTag() :tag("Entity") { }
		CompTag(const String& tag) :tag(tag) { }

		String tag;

		void WritePrivateData(std::ofstream& writer);
		void ReadPrivateData(std::ifstream& reader);
	};
}