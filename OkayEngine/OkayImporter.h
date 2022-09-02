#pragma once

#include <assimp/importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "SkeletalMesh.h"
#include "Mesh.h"

// Private class with friend class acts like a private namespace
class Importer
{
private:
	friend class Assets; 

	static bool Load(const std::string_view& meshFile, Okay::VertexData& outData, std::string* texPath);

	static bool WriteOkayAsset(const std::string& meshFile, const Okay::VertexData& vertexData);

	static bool LoadOkayAsset(const std::string& meshFile, Okay::VertexData& vertexData);
};
