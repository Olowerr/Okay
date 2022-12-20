#pragma once
#include <vector>

#include "Engine/Graphics/Shader.h"

//#include "Graphics/SkeletalMesh.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/MeshComponent.h"

namespace Okay
{
	class ContentBrowser;

	class Renderer
	{
	public:
		Renderer(ContentBrowser& content);
		~Renderer();
		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		void submit(Okay::MeshComponent* pMesh, Okay::Transform* pTransform);
		//void SumbitSkeletal(Okay::CompSkeletalMesh* pMesh, Okay::Transform* pTransform);
		//void SubmitLight(Okay::CompPointLight* pLight, Okay::Transform* pTransform);

		void newFrame();

		void shutdown();
		void render();


	private:
		ContentBrowser& content;
		ID3D11DeviceContext* pDevContext;

		struct RenderMesh
		{
			Okay::MeshComponent* mesh;
			Okay::Transform* transform;
		};
		std::vector<RenderMesh> meshesToRender;
		size_t numActiveMeshes;

		struct RenderSkeleton // Will change
		{
			Okay::MeshComponent* mesh;
			Okay::Transform* transform;
		};
		std::vector<RenderSkeleton> skeletonsToRender;
		size_t numActiveSkeletons;

		struct GPUPointLight
		{
			//Okay::CompPointLight lightData;
			glm::vec3 pos;
		};

		std::vector<GPUPointLight> lights;
		size_t numLights;

	private: // Buffers

		ID3D11Buffer* pViewProjectBuffer;
		ID3D11Buffer* pWorldBuffer;
		ID3D11Buffer* pMaterialBuffer;

		ID3D11Buffer* pPointLightBuffer;
		ID3D11ShaderResourceView* pPointLightSRV;
		ID3D11Buffer* pLightInfoBuffer;
		void expandPointLights();

		ID3D11RenderTargetView* bbRTV;

	private: // Piplines
		void bindNecessities();
		Okay::Shader defaultPixelShader;
		D3D11_VIEWPORT viewport;

		// Static meshes
		void bindMeshPipeline();
		ID3D11InputLayout* pMeshIL;
		ID3D11VertexShader* pMeshVS;

		// Skeletal meshes
		void bindSkeletalPipeline();
		ID3D11InputLayout* pAniIL;
		ID3D11VertexShader* pAniVS;

	private: // Create Shaders
		void createVertexShaders();
		void createPixelShaders();
	};
}




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