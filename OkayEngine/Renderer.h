#pragma once
#include <vector>

#include "ShaderModel.h"
#include "Camera.h"
#include "Components.h"
#include "SkeletalMesh.h"

#include <assimp/importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <unordered_map>

class Renderer	
{
public:
	Renderer();
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void Resize();

	void Submit(Okay::CompMesh* pMesh, Okay::CompTransform* pTransform);
	void SubmitLight(Okay::CompPointLight* pLight, Okay::CompTransform* pTransform);

	void NewFrame();

	void Shutdown();
	void Render();
	

private:
	std::unique_ptr<Okay::ShaderModel> shaderModel;
	std::unique_ptr<Okay::Camera> mainCamera;

	struct RenderMesh
	{
		Okay::CompMesh* mesh;
		Okay::CompTransform* transform;
	};
	std::vector<RenderMesh> meshesToRender;
	size_t numActive;


	struct GPUPointLight
	{
		Okay::Float3 pos;
		Okay::CompPointLight lightData;
	};

	std::vector<GPUPointLight> lights;
	size_t numLights; 

private: // DX11 Specific
	ID3D11DeviceContext* pDevContext;

	ID3D11Buffer* pViewProjectBuffer;
	ID3D11Buffer* pWorldBuffer;
	ID3D11Buffer* pMaterialBuffer;

	ID3D11Buffer* pPointLightBuffer;
	ID3D11ShaderResourceView* pPointLightSRV;
	ID3D11Buffer* pLightInfoBuffer;
	bool ExpandPointLights();

private: // Pipline
	ID3D11InputLayout* pInputLayout;
	ID3D11VertexShader* pVertexShader;
	ID3D11HullShader* pHullShader;     // Disabled
	ID3D11DomainShader* pDomainShader; // Disabled

	void Bind();

private: // Create Shaders
	bool CreateVS();
	bool CreateHS();
	bool CreateDS();






	// TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP 
	struct TimeStamp
	{
		TimeStamp()
			:scale(1.f, 1.f, 1.f) { }

		float time = 0.f;
		aiVector3t<float> pos;
		aiQuaterniont<float> rot;
		aiVector3t<float> scale;
	};

	struct Joint
	{
		std::string name;
		int parentIdx = -1;
		DirectX::XMMATRIX bindPose{};
		DirectX::XMMATRIX matrix{};
		std::vector<TimeStamp> stamps;
	};


	
	std::vector<DirectX::XMFLOAT4X4> aniMatrices;
	ID3D11Buffer* aniBuffer;
	ID3D11ShaderResourceView* aniSRV;
	float aniDuration;
	float aniTime;

	ID3D11VertexShader* aniVS = nullptr;
	ID3D11InputLayout*  aniIL;
	std::unique_ptr<Okay::SkeletalMesh> goblin;
	std::vector<Joint> joints;

	int FindJointIndex(std::vector<Joint>& joints, std::string_view name)
	{
		for (int i = 0; i < (int)joints.size(); i++)
		{
			if (joints[i].name == name)
				return i;
		}

		return -1;
	}

	aiNode* GetParentNode(std::vector<Joint>& joints, aiNode* child)
	{
		// Is root?
		if (!child->mParent)
			return nullptr;

		// Is the parent node a joint?
		if (FindJointIndex(joints, child->mParent->mName.C_Str()) != -1)
			return child->mParent;

		// Continue searching...
		return GetParentNode(joints, child->mParent);
	}

	void SetParents(std::vector<Joint>& joints, aiNode* node)
	{
		int currentJointIdx = FindJointIndex(joints, node->mName.C_Str());
		if (currentJointIdx != -1)
		{
			aiNode* pParent = GetParentNode(joints, node);
			if (pParent)
			{
				joints[currentJointIdx].parentIdx = FindJointIndex(joints, pParent->mName.C_Str());
			}
		}

		for (UINT i = 0; i < node->mNumChildren; i++)
			SetParents(joints, node->mChildren[i]);
	}

	aiNodeAnim* FindAniNode(std::vector<aiNodeAnim*>& vec, std::string_view name, const std::string_view component)
	{
		for (aiNodeAnim* node : vec)
		{
			std::string_view nodeName = node->mNodeName.C_Str();

			if (nodeName.find(name) != -1)
			{
				if (nodeName.find(component) != -1)
					return node;
			}
		}

		return nullptr;
	}

	void FillNodes(std::unordered_map<std::string_view, aiNode*>& nodes, aiNode* root)
	{
		for (UINT i = 0; i < root->mNumChildren; i++)
		{
			nodes[root->mChildren[i]->mName.C_Str()] = root->mChildren[i];
			FillNodes(nodes, root->mChildren[i]);
		}
	}

	void CreateSkeletal()
	{
		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile("..\\Content\\Meshes\\ani\\StickANi.fbx",
			aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices);

		if (!pScene)
			return;

		auto ani = pScene->mAnimations[0];
		auto mesh = pScene->mMeshes[0];

		aniDuration = (float)ani->mDuration;
		aniTime = 0.f;

		std::unordered_map<std::string_view, aiNode*> nodes;


		std::vector<aiNodeAnim*> aniNodes(ani->mNumChannels);
		memcpy(aniNodes.data(), ani->mChannels, sizeof(ani->mChannels) * ani->mNumChannels);

		aniMatrices.resize(mesh->mNumBones);
		joints.resize(mesh->mNumBones);
		for (UINT i = 0; i < mesh->mNumBones; i++)
		{
			joints[i].name = mesh->mBones[i]->mName.C_Str();
			
			aiNodeAnim* traChannel = FindAniNode(aniNodes, joints[i].name.c_str(), "Translation");
			aiNodeAnim* rotChannel = FindAniNode(aniNodes, joints[i].name.c_str(), "Rotation");
			aiNodeAnim* scaChannel = FindAniNode(aniNodes, joints[i].name.c_str(), "Scaling");

			const size_t numKeys = traChannel ? traChannel->mNumPositionKeys : rotChannel ? rotChannel->mNumRotationKeys : scaChannel ? scaChannel->mNumScalingKeys : 0;
			joints[i].stamps.resize(numKeys);

			for (size_t k = 0; k < numKeys; k++)
			{
				if (traChannel) joints[i].stamps[k].time = (float)traChannel->mPositionKeys[k].mTime;
				else if (rotChannel) joints[i].stamps[k].time = (float)rotChannel->mRotationKeys[k].mTime;
				else if (scaChannel) joints[i].stamps[k].time = (float)scaChannel->mScalingKeys[k].mTime;

				if (traChannel) joints[i].stamps[k].pos = traChannel->mPositionKeys[k].mValue;
				if (rotChannel) joints[i].stamps[k].rot = rotChannel->mRotationKeys[k].mValue;
				if (scaChannel) joints[i].stamps[k].scale = scaChannel->mScalingKeys[k].mValue;
			}
		}

		SetParents(joints, pScene->mRootNode);

		Okay::SkeletalMesh::VertexData data;

		// size is temp
		data.weights.resize(mesh->mNumVertices);

		for (UINT i = 0; i < mesh->mNumBones; i++)
		{
			memcpy(&joints[i].bindPose, &mesh->mBones[i]->mOffsetMatrix, sizeof(DirectX::XMFLOAT4X4));
			//joints[i].bindPose = DirectX::XMMatrixInverse(nullptr, joints[i].bindPose);
			//joints[i].bindPose = DirectX::XMMatrixTranspose(joints[i].bindPose);


			for (UINT k = 0; k < mesh->mBones[i]->mNumWeights; k++)
			{
				aiVertexWeight& aiWeight = mesh->mBones[i]->mWeights[k];
				Okay::SkinnedVertex& currVertex = data.weights[aiWeight.mVertexId];

				if		(currVertex.weight[0] == 0.f) { currVertex.weight[0] = aiWeight.mWeight; currVertex.jointIdx[0] = i; }
				else if (currVertex.weight[1] == 0.f) { currVertex.weight[1] = aiWeight.mWeight; currVertex.jointIdx[1] = i; }
				else if (currVertex.weight[2] == 0.f) { currVertex.weight[2] = aiWeight.mWeight; currVertex.jointIdx[2] = i; }
				else if (currVertex.weight[3] == 0.f) { currVertex.weight[3] = aiWeight.mWeight; currVertex.jointIdx[3] = i; }
			}
		}

		data.position.resize(mesh->mNumVertices);
		memcpy(data.position.data(), mesh->mVertices, sizeof(Okay::Float3) * mesh->mNumVertices);

		// Vertex UV & mesh
		data.uvNormal.resize(mesh->mNumVertices);
		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			data.uvNormal[i].normal.x = mesh->mNormals[i].x;
			data.uvNormal[i].normal.y = mesh->mNormals[i].y;
			data.uvNormal[i].normal.z = mesh->mNormals[i].z;
			
			data.uvNormal[i].uv.x = mesh->mTextureCoords[0][i].x;
			data.uvNormal[i].uv.y = mesh->mTextureCoords[0][i].y;
		}

		// Indices
		UINT counter = 0;
		const UINT NumIndices = mesh->mNumFaces * 3;

		data.indices.resize(NumIndices);
		for (UINT i = 0; i < mesh->mNumFaces; i++)
		{
			data.indices[counter++] = mesh->mFaces[i].mIndices[0];
			data.indices[counter++] = mesh->mFaces[i].mIndices[1];
			data.indices[counter++] = mesh->mFaces[i].mIndices[2];
		}


		goblin = std::make_unique<Okay::SkeletalMesh>(data);
	}

	void CalculateAnimation()
	{
		size_t currentStamp = 0;
		for (size_t i = 0; i < joints[0].stamps.size(); i++)
		{
			if (aniTime < joints[0].stamps[i].time)
			{
				currentStamp = i - 1;
				if (currentStamp == -1)
					currentStamp = 0;

				break;
			}
		}
		
		printf("Stamp: %zd\nTime: %f\n", currentStamp, aniTime);

		using namespace DirectX;
		TimeStamp& root = joints[0].stamps[currentStamp];
		joints[0].matrix =
			(XMMatrixScaling(root.scale.x, root.scale.y, root.scale.z) *
			XMMatrixRotationQuaternion(XMVectorSet(root.rot.x, root.rot.y, root.rot.z, root.rot.w)) *
			XMMatrixTranslation(root.pos.x, root.pos.y, root.pos.z)) *
			joints[0].bindPose;

		for (size_t i = 1; i < joints.size(); i++)
		{
			TimeStamp& stamp = joints[i].stamps[currentStamp];

			joints[i].matrix = joints[joints[i].parentIdx].matrix *
				(XMMatrixScaling(stamp.scale.x, stamp.scale.y, stamp.scale.z) *
				XMMatrixRotationQuaternion(XMVectorSet(stamp.rot.x, stamp.rot.y, stamp.rot.z, stamp.rot.w)) *
				XMMatrixTranslation(stamp.pos.x, stamp.pos.y, stamp.pos.z)) *
				joints[i].bindPose;

		}

		for (size_t i = 0; i < joints.size(); i++)
			XMStoreFloat4x4(&aniMatrices[i], joints[i].matrix);
		
		DX11::UpdateBuffer(aniBuffer, aniMatrices.data(), UINT(sizeof(XMFLOAT4X4) * aniMatrices.size()));

	}
};


/*
	TODO (in order):
	Create Camera Class
	Force Render triangle


	Implement Importer (Seperate project) (.fbx & .obj)
	Materials somewhere here..?
	Implement rendering without instancing
	
	Include Entt
	Create Okay::MeshComponent & Okay::Transform (component ?)
	Render by Entity

	Create Scene Class
	

*/