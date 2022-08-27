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


	

	struct Joint
	{
		
	};


	Assimp::Importer importer;
	const aiScene* pScene;
	std::vector<aiNode*> nodes;
	std::unordered_map<std::string_view, aiNode*> nodesMap;
	std::vector<aiNodeAnim*> aniNodes;

	std::vector<DirectX::XMFLOAT4X4> aniMatrices;
	ID3D11Buffer* aniBuffer;
	ID3D11ShaderResourceView* aniSRV;
	float aniDurationS;
	float aniTime;
	float tickLengthS;

	ID3D11VertexShader* aniVS = nullptr;
	ID3D11InputLayout*  aniIL;
	std::unique_ptr<Okay::SkeletalMesh> goblin;
	std::vector<Joint> joints;

	int FindJointIndex(std::vector<Joint>& joints, std::string_view name);
	aiNode* GetParentNode(std::vector<Joint>& joints, aiNode* child);
	void SetParents(std::vector<Joint>& joints, aiNode* node);
	aiNodeAnim* FindAniNode(std::vector<aiNodeAnim*>& vec, std::string_view name, const std::string_view component);
	void FillNodes(std::vector<aiNode*>& nodes, aiNode* root);
	void FillNodes(std::unordered_map<std::string_view, aiNode*>& nodes, aiNode* root);
	void CreateSkeletal();
	void CalculateAnimation(float dt);

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