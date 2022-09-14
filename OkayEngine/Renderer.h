#pragma once
#include <vector>

#include "ShaderModel.h"
#include "Components.h"
#include "SkeletalMesh.h"

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

	// temp
	void SetCamera(Okay::CompCamera* camera)
	{
		mainCamera = camera;
	}

private:
	std::unique_ptr<Okay::ShaderModel> shaderModel;
	Okay::CompCamera* mainCamera;

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