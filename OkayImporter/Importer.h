#pragma once
#include <fstream>
#include <fbxsdk.h>


#include <DirectXMath.h>

namespace Okay
{
	using Float3 = DirectX::XMFLOAT3;
	using Float2 = DirectX::XMFLOAT2;

	struct UVNormal
	{
		UVNormal() = default;
		~UVNormal() = default;

		Float2 uv;
		Float3 normal;
	};
}

void print()
{
	// Set up
	FbxManager* manen = FbxManager::Create();

	FbxIOSettings* ios = FbxIOSettings::Create(manen, IOSROOT);
	manen->SetIOSettings(ios);

	FbxScene* scene = FbxScene::Create(manen, "My Scene");

	
	// Load scene
	char path[] = "box.fbx";

	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor, lSDKMinor, lSDKRevision;

	int lAnimStackCount;
	bool lStatus;
	
	//FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	FbxImporter* importer = FbxImporter::Create(manen, "My Importer");

	importer->Initialize(path, -1, ios);

	//importer->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	importer->Import(scene);
	// "Check the scene integrity" nahh

	importer->Destroy();

	FbxGeometryConverter converter(manen);
	converter.Triangulate(scene, true);

	FbxNode* root = scene->GetRootNode();

	Okay::Float3 vert;

	for (int i = 0; i < root->GetChildCount(); ++i)
	{
		FbxNode* child = root->GetChild(i);
		switch (child->GetNodeAttribute()->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
		{
			FbxMesh* mesh = (FbxMesh*)child->GetNodeAttribute();
			
			FbxVector4* controlPoints = mesh->GetControlPoints();

			for (int k = 0; k < mesh->GetPolygonCount(); ++k) // per face
			{

				for (int j = 0; j < 3; j++) // per vertex in face
				{
					int controlPIndex = mesh->GetPolygonVertex(k, j);

					vert.x = controlPoints[controlPIndex][0];
					vert.y = controlPoints[controlPIndex][1];
					vert.z = controlPoints[controlPIndex][2];
					
				}

			}
		}
		}
	}
};