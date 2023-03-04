#pragma once
#include <vector>

#include "Assets/Shader.h"

#include "Engine/Components/PointLight.h"
#include "Engine/Components/DirectionalLight.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/SkyLight.h"

#include "Engine/Application/Entity.h"

#include <memory>

namespace Okay
{
	class RenderTexture;
	class ContentBrowser;

	struct MeshComponent;
	struct Transform;

	/*
	* Used to pair an asset to a transform during rendering
	* For example a mesh to a transform or a pointLight to a position
	*/
	template<typename T1, typename T2>
	struct RenPair // TODO: Find better name
	{
		RenPair(T1 asset, T2 transform)
			:asset(asset), transform(transform) 
		{ }

		T1 asset;
		T2 transform;
	};

	struct LightInfo
	{
		uint32_t numPointLights = 0u;
		uint32_t numDirLights = 0u;

		uint32_t padding[2]{};
	};

	struct GPUCamera // Change to multiple memcpys ?
	{
		glm::mat4 viewProjMatrix;

		glm::vec3 pos;
		float paddding1;

		glm::vec3 direction;
		float paddding2;
	};

	class Renderer
	{
	public:
		
		static void init();
		static const uint32_t DEFAULT_SHADER_IDX = 0u; // TODO: Fix, kinda sus no?

		Renderer(RenderTexture* target);
		~Renderer();

		void submit(const MeshComponent& mesh, const Transform& transform);
		void submit(const PointLight& light, const Transform& transform);
		void submit(const DirectionalLight& light, const Transform& transform);

		void setRenderTexture(RenderTexture* pRenderTexture);
		inline void setCamera(Entity cameraEntity);
		inline void setSkyLight(Entity skyEntity);

		void newFrame();
		void render();
		void setWireframe(bool wireFrame);


	private: // Misc
		void onTargetResize(uint32_t width, uint32_t height);
		RenderTexture* pRenderTarget;
		Entity cameraEntity;
		Entity skyEntity;

		ID3D11DeviceContext* pDevContext;
		ID3D11RasterizerState* pWireframeRS;
		D3D11_VIEWPORT viewport;

	private: // Assets to render
		using MeshPair			= RenPair<const MeshComponent&, const glm::mat4&>;
		using PointLightPair	= RenPair<const PointLight&, const Transform&>;
		using DirLightPair		= RenPair<const DirectionalLight&, const Transform&>;

		LightInfo lightInfo;
		GPUCamera camData;

		std::vector<MeshPair> meshes;
		std::vector<PointLightPair> pointLights;
		std::vector<DirLightPair> dirLights;

		void updatePointLightsBuffer();
		void updateDirLightsBuffer();
	
		void calculateCameraMatrix();

	private: // Static PipelineResources

		struct PipelineResources
		{
			PipelineResources() = default;
			~PipelineResources();

			// Buffers --- 
			ID3D11Buffer* pCameraBuffer = nullptr;
			ID3D11Buffer* pWorldBuffer = nullptr;
			ID3D11Buffer* pMaterialBuffer = nullptr;
			ID3D11Buffer* pShaderDataBuffer = nullptr;
			ID3D11Buffer* pSkyLightDataBuffer = nullptr;

			ID3D11Buffer* pLightInfoBuffer = nullptr;
			ID3D11Buffer* pPointLightBuffer = nullptr;
			ID3D11Buffer* pDirLightBuffer = nullptr;
			ID3D11ShaderResourceView* pPointLightSRV = nullptr;
			ID3D11ShaderResourceView* pDirLightSRV = nullptr;
			uint32_t maxPointLights = 0u;
			uint32_t maxDirLights = 0u;


			uint32_t skyboxMeshId = INVALID_UINT;

			// Input layouts
			ID3D11InputLayout* pPosIL = nullptr;
			ID3D11InputLayout* pPosUvNormIL = nullptr;
			ID3D11InputLayout* pPosUvNormJidxWeightIL = nullptr;

			// Vertex shaders
			ID3D11VertexShader* pMeshVS = nullptr;
			ID3D11VertexShader* pSkeletalVS = nullptr;
			ID3D11VertexShader* pSkyBoxVS = nullptr;

			// Rasterizer states
			ID3D11RasterizerState* pWireframeRS = nullptr;
			ID3D11RasterizerState* pNoCullRS = nullptr;

			// Pixel shaders
			ID3D11PixelShader* pSkyBoxPS = nullptr;

			// Depth Stencils
			ID3D11DepthStencilState* pLessEqualDSS = nullptr;
		};

		static std::unique_ptr<PipelineResources> pipeline;

		static void expandPointLights();
		static void expandDirLights();

		static void bindMeshPipeline();
		static void bindSkeletalPipeline();

	};


	inline void Renderer::setCamera(Entity cameraEntity)
	{
		OKAY_ASSERT(cameraEntity.isValid(), "Camera entity isn't valid");
		OKAY_ASSERT(cameraEntity.hasComponent<Camera>(), "Camera entity doesn't have a Camera Component");
		this->cameraEntity = cameraEntity;
	}

	inline void Renderer::setSkyLight(Entity skyEntity)
	{
		OKAY_ASSERT(skyEntity.hasComponent<SkyLight>(), "Sky Entity entity doesn't have a SkyLight Component");
		this->skyEntity = skyEntity;
	}
}