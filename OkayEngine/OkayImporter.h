#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX

#include <assimp/importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Importer
{
public:
	friend class Assets; // Only accessible to Assets

	struct VertexData
	{
		std::vector<Okay::Float3> position;
		std::vector<Okay::UVNormal> uvNormal;
		std::vector<UINT> indices;
	};

	struct MeshData
	{
		UINT numVertex;
		UINT numIndex;
	};

	static void Load(const std::string& meshFile, VertexData& outData);

	static void WriteBinary(const std::string& meshFile, VertexData& vertexData);

};

inline void Importer::Load(const std::string& meshFile, VertexData& outData)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile("../Assets/Meshes/TempObjFbx/" + meshFile,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices);

	if (!pScene)
		return;

	aiNode* pNode = pScene->mRootNode;
	aiMesh* pMesh = pScene->mMeshes[0];	////


	// Vertex Positions
	outData.position.clear();
	outData.position.resize(pMesh->mNumVertices);
	memcpy(outData.position.data(), pMesh->mVertices, sizeof(Okay::Float3) * pMesh->mNumVertices);


	// Vertex UV & Normals
	outData.uvNormal.clear();
	outData.uvNormal.resize(pMesh->mNumVertices);
	for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
	{
		outData.uvNormal[i].normal.x = pMesh->mNormals[i].x;
		outData.uvNormal[i].normal.y = pMesh->mNormals[i].z;
		outData.uvNormal[i].normal.z = pMesh->mNormals[i].y;

		outData.uvNormal[i].uv.x = pMesh->mTextureCoords[0][i].x;
		outData.uvNormal[i].uv.y = pMesh->mTextureCoords[0][i].y;
	}


	// Indices
	UINT counter = 0;
	const UINT NumIndices = pMesh->mNumFaces * 3;

	outData.indices.clear();
	outData.indices.resize(NumIndices);
	for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
	{
		outData.indices[counter++] = pMesh->mFaces[i].mIndices[0];
		outData.indices[counter++] = pMesh->mFaces[i].mIndices[1];
		outData.indices[counter++] = pMesh->mFaces[i].mIndices[2];
	}


	WriteBinary(meshFile, outData);
}

inline void Importer::WriteBinary(const std::string& meshFile, VertexData& vertexData)
{
	std::string fileName = meshFile;
	fileName = fileName.substr(0, fileName.find_last_of('.')) + ".okayAsset";

	std::ofstream writer("../Assets/Meshes/" + fileName, std::ios::binary | std::ios::trunc);

	MeshData info;
	info.numVertex = (UINT)vertexData.position.size();
	info.numIndex = (UINT)vertexData.indices.size();

	writer.write((const char*)&info, sizeof(MeshData));
	writer.write((const char*)vertexData.position.data(), sizeof(Okay::Float3) * info.numVertex);
	writer.write((const char*)vertexData.uvNormal.data(), sizeof(Okay::UVNormal) * info.numVertex);
	writer.write((const char*)vertexData.indices.data(), sizeof(UINT) * info.numIndex);

	writer.close();
}