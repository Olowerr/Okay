#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX

#include <assimp/importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Importer
{
private:
	friend class Assets; // Only accessible to Assets

	struct MeshData
	{
		UINT numVertex;
		UINT numIndex;
	};

	static bool Load(const std::string& meshFile, Okay::VertexData& outData);

	static bool WriteOkayAsset(const std::string& meshFile, const Okay::VertexData& vertexData);

	static bool LoadOkayAsset(const std::string& meshFile, Okay::VertexData& vertexData);
};

inline bool Importer::Load(const std::string& meshFile, Okay::VertexData& outData)
{
	Assimp::Importer importer;

	std::string path;
	if (meshFile.find('/') == -1) // Absolute path?
		path = "../Assets/Meshes/TempObjFbx/" + meshFile;
	else
		path = meshFile;

	const aiScene* pScene = importer.ReadFile(path,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices);

	VERIFY(pScene);

	//aiNode* pNode = pScene->mRootNode;
	aiMesh* pMesh = pScene->mMeshes[0];	////


	// Vertex Positions
	outData.position.resize(pMesh->mNumVertices);
	memcpy(outData.position.data(), pMesh->mVertices, sizeof(Okay::Float3) * pMesh->mNumVertices);


	// Vertex UV & Normals
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

	outData.indices.resize(NumIndices);
	for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
	{
		outData.indices[counter++] = pMesh->mFaces[i].mIndices[0];
		outData.indices[counter++] = pMesh->mFaces[i].mIndices[1];
		outData.indices[counter++] = pMesh->mFaces[i].mIndices[2];
	}

	return WriteOkayAsset(meshFile, outData);
}

inline bool Importer::WriteOkayAsset(const std::string& filePath, const Okay::VertexData& vertexData)
{
	std::string fileName = filePath.substr(filePath.find_last_of('/') + 1);
	fileName = fileName.substr(0, fileName.find_last_of('.')) + ".okayAsset";

	std::ofstream writer("../Assets/Meshes/" + fileName, std::ios::binary | std::ios::trunc);
	VERIFY(writer);

	MeshData info{};
	info.numVertex = (UINT)vertexData.position.size();
	info.numIndex = (UINT)vertexData.indices.size();

	writer.write((const char*)&info, sizeof(MeshData));
	writer.write((const char*)vertexData.position.data(), sizeof(Okay::Float3) * info.numVertex);
	writer.write((const char*)vertexData.uvNormal.data(), sizeof(Okay::UVNormal) * info.numVertex);
	writer.write((const char*)vertexData.indices.data(), sizeof(UINT) * info.numIndex);
	
	writer.close();

	return true;
}

inline bool Importer::LoadOkayAsset(const std::string& filePath, Okay::VertexData& vertexData)
{
	std::string fileName = filePath;
	fileName = fileName.substr(0, fileName.find_last_of('.')) + ".okayAsset";

	std::ifstream reader("../Assets/Meshes/" + fileName, std::ios::binary);
	VERIFY(reader);

	MeshData readData{};
	reader.read((char*)&readData, sizeof(MeshData));

	vertexData.position.resize(readData.numVertex);
	vertexData.uvNormal.resize(readData.numVertex);
	vertexData.indices.resize(readData.numIndex);
	
	reader.read((char*)vertexData.position.data(), sizeof(Okay::Float3) * readData.numVertex);
	reader.read((char*)vertexData.uvNormal.data(), sizeof(Okay::UVNormal) * readData.numVertex);
	reader.read((char*)vertexData.indices.data(), sizeof(UINT) * readData.numIndex);

	reader.close();

	return true;
}