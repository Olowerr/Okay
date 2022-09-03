#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX

#include <assimp/cimport.h>
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

	static bool Load(const std::string_view& filePath, Okay::VertexData& outData, std::string* texPath);

	static bool LoadSkeletal(const std::string_view& filePath, Okay::SkeletalVertexData& outData);

	static bool WriteOkayAsset(const std::string& filePath, const Okay::VertexData& vertexData);

	static bool LoadOkayAsset(const std::string& filePath, Okay::VertexData& vertexData);


	// Helper Functions
	static aiNodeAnim* FindAnimNode(aiNodeAnim** nodes, UINT numNodesa, std::string_view name);
	static bool FixJoint(Okay::Joint& joint, aiNode* pRootNode);
	static aiNode* FindTraNode(aiNode* pParent, std::string_view name);

	static int FindJointIndex(std::vector<Okay::Joint>& joints, std::string_view name);
	static aiNode* GetJointParentNode(std::vector<Okay::Joint>& joints, aiNode* child);
	static void SetParents(std::vector<Okay::Joint>& joints, aiNode* node);
};

inline aiNodeAnim* Importer::FindAnimNode(aiNodeAnim** nodes, UINT numNodes, std::string_view name)
{
	for (UINT i = 0; i < numNodes; i++)
	{
		if (nodes[i]->mNodeName.C_Str() == name)
			return nodes[i];
	}

	return nullptr;
}

inline bool Importer::FixJoint(Okay::Joint& joint, aiNode* pRootNode)
{
	std::string name = std::string(joint.name) + "_$AssimpFbx$_Translation";
	aiNode* pNode = FindTraNode(pRootNode, name);
	joint.stamps.resize(1);

	if (!pNode)
		return false;

	joint.stamps[0].time = 0.f;

	joint.stamps[0].pos.x = pNode->mTransformation.a4;
	joint.stamps[0].pos.y = pNode->mTransformation.b4;
	joint.stamps[0].pos.z = pNode->mTransformation.c4;

	return true;
}

inline aiNode* Importer::FindTraNode(aiNode* parent, std::string_view jointName)
{
	aiNode* ptr = nullptr;

	for (size_t i = 0; i < parent->mNumChildren; i++)
	{
		if (parent->mChildren[i]->mName.C_Str() == jointName)
			ptr = parent->mChildren[i];

		else if (!ptr)
			ptr = FindTraNode(parent->mChildren[i], jointName);
	}

	return ptr;
}

inline int Importer::FindJointIndex(std::vector<Okay::Joint>& joints, std::string_view name)
{
	for (int i = 0; i < (int)joints.size(); i++)
	{
		if (joints[i].name == name)
			return i;
	}

	return -1;
}

inline aiNode* Importer::GetJointParentNode(std::vector<Okay::Joint>& joints, aiNode* child)
{
	// Is root?
	if (!child->mParent)
		return nullptr;

	// Is the parent node a joint?
	if (FindJointIndex(joints, child->mParent->mName.C_Str()) != -1)
		return child->mParent;

	// Continue searching...
	return GetJointParentNode(joints, child->mParent);
}

inline void Importer::SetParents(std::vector<Okay::Joint>& joints, aiNode* node)
{
	int currentJointIdx = FindJointIndex(joints, node->mName.C_Str());
	if (currentJointIdx != -1)
	{
		aiNode* pParent = GetJointParentNode(joints, node);
		if (pParent)
			joints[currentJointIdx].parentIdx = FindJointIndex(joints, pParent->mName.C_Str());
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
		SetParents(joints, node->mChildren[i]);
}