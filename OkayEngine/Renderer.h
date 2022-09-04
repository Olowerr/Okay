#pragma once
#include <vector>

#include "ShaderModel.h"
#include "Camera.h"
#include "Components.h"
#include "SkeletalMesh.h"

#define FORCE_ANIMATION 0

#if FORCE_ANIMATION == 1
#include <assimp/cimport.h>
#include <assimp/importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#endif

#include <unordered_map>
#include <tuple>

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
	void SumbitSkeletal(Okay::CompSkeletalMesh* pMesh, Okay::CompTransform* pTransform);
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

	// Testing std::pair..
	std::vector<std::pair<Okay::CompSkeletalMesh*, Okay::CompTransform*>> skeletalMeshes;
	size_t numSkeletalActive;

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
	ID3D11InputLayout* pMeshIL;
	ID3D11VertexShader* pMeshVS;
	ID3D11HullShader* pHullShader;     // Disabled
	ID3D11DomainShader* pDomainShader; // Disabled

	void BindBasics();
	void BindMeshPipeline();
	void BindSkeletalPipeline();

	// Animation
	ID3D11InputLayout* pAniIL;
	ID3D11VertexShader* pAniVS;

private: // Create Shaders
	bool CreateVS();
	bool CreateHS();
	bool CreateDS();





#if FORCE_ANIMATION == 1
	// TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP 


	

	struct Joint
	{
		
	};



	std::vector<DirectX::XMFLOAT4X4> aniMatrices;
	ID3D11Buffer* aniBuffer;
	ID3D11ShaderResourceView* aniSRV;
	float aniDurationS;
	float aniTime;
	float tickLengthS;

	ID3D11VertexShader* aniVS;
	ID3D11InputLayout* aniIL;
	std::unique_ptr<Okay::SkeletalMesh> goblin;
	std::vector<Joint> joints;

	aiNodeAnim* FindAnimNode(aiNodeAnim** nodes, UINT num, std::string_view name)
	{
		for (UINT i = 0; i < num; i++)
		{
			if (nodes[i]->mNodeName.C_Str() == name)
				return nodes[i];
		}

		return nullptr;
	}
	aiNode* FindTraNode(aiNode* parent, const std::string& jointName)
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

	bool FixJoint(Joint& joint, aiNode* root)
	{
		std::string name = joint.name + "_$AssimpFbx$_Translation";
		aiNode* pNode = FindTraNode(root, name);
		joint.stamps.resize(1);

		if (!pNode)
			return false;

		joint.stamps[0].time = 0.f;

		joint.stamps[0].pos.x = pNode->mTransformation.a4;
		joint.stamps[0].pos.y = pNode->mTransformation.b4;
		joint.stamps[0].pos.z = pNode->mTransformation.c4;

		return true;
	}

	int FindJointIndex(std::vector<Joint>& joints, std::string_view name);
	aiNode* GetParentNode(std::vector<Joint>& joints, aiNode* child);
	void SetParents(std::vector<Joint>& joints, aiNode* node);

	void CreateSkeletal();
	void CalculateAnimation(float dt);
#endif
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