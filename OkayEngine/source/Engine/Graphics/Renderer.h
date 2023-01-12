#pragma once
#include <vector>

#include "Shader.h"
#include "Engine/Components/PointLight.h"

namespace Okay
{
	class RenderTexture;
	class ContentBrowser;
	class Entity;

	struct MeshComponent;
	struct Transform;

	class Renderer
	{
	public:
		static const uint32_t DEFAULT_SHADER_IDX = 0u;

		Renderer(const RenderTexture* target, ContentBrowser& content);
		~Renderer();
		void shutdown();

		void submit(const MeshComponent* pMesh, const Transform* pTransform);
		//void SumbitSkeletal(Okay::CompSkeletalMesh* pMesh, Okay::Transform* pTransform);
		void submitPointLight(const PointLight& pLight, const Transform& pTransform);

		void setRenderTexture(const RenderTexture* pRenderTexture);

		void newFrame();
		void render(const Entity& cameraEntity);


	private:
		const RenderTexture* pRenderTarget;
		const ContentBrowser& content;
		ID3D11DeviceContext* pDevContext;

		struct RenderMesh
		{
			const MeshComponent* pMesh = nullptr;
			const Transform* pTransform = nullptr;
		};
		std::vector<RenderMesh> meshesToRender;
		size_t numActiveMeshes;

		struct RenderSkeleton // Will change
		{
			const MeshComponent* pMesh = nullptr;
			const Transform* pTransform = nullptr;
		};
		std::vector<RenderSkeleton> skeletonsToRender;
		size_t numActiveSkeletons;

		struct GPUPointLight
		{
			PointLight lightData;
			glm::vec3 pos;
		};
		std::vector<GPUPointLight> lights;
		size_t numPointLights;
		void expandPointLights();

	private: // Buffers

		ID3D11Buffer* pViewProjectBuffer;
		ID3D11Buffer* pWorldBuffer;
		ID3D11Buffer* pMaterialBuffer;
		ID3D11Buffer* pShaderDataBuffer;

		ID3D11Buffer* pPointLightBuffer;
		ID3D11ShaderResourceView* pPointLightSRV;
		ID3D11Buffer* pLightInfoBuffer;

		ID3D11RenderTargetView* bbRTV;

	private: // Piplines
		void bindNecessities();
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