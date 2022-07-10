#pragma once

#include <assimp/importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "OkayVertices.h"

class Importer
{
public:

	static void Test()
	{
		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile("../Resources/Meshes/cube.fbx",
			aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices); 		

		if (!pScene)
			return;

		aiNode* pNode = pScene->mRootNode;
		aiMesh* pMesh = pScene->mMeshes[0];	////
	
		for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
		{
			pMesh->mVertices[i];
			pMesh->mTextureCoords[0][i];
			pMesh->mNormals[i];
		}

		for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
		{
			auto& face = pMesh->mFaces[i];
			face.mIndices[0];
			face.mIndices[1];
			face.mIndices[2];
		}

	}

};