#pragma once
#include <vector>

#include "Assets/Shader.h"
#include "Engine/Components/PointLight.h"
#include "Engine/Components/DirectionalLight.h"

namespace Okay
{
	class RenderTexture;
	class ContentBrowser;
	class Entity;

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
		static const uint32_t DEFAULT_SHADER_IDX = 0u;

		Renderer(RenderTexture* target, ContentBrowser& content);
		~Renderer();
		void shutdown();

		void submit(const MeshComponent& mesh, const Transform& transform);
		void submit(const PointLight& light, const Transform& transform);
		void submit(const DirectionalLight& light, const Transform& transform);

		void setRenderTexture(RenderTexture* pRenderTexture);

		void newFrame();
		void render(Entity cameraEntity);
		void setWireframe(bool wireFrame);


	private: // Misc
		void onTargetResize();
		RenderTexture* pRenderTarget;
		const ContentBrowser& content;
		ID3D11DeviceContext* pDevContext;

	private: // Assets to render
		using MeshPair			= RenPair<const MeshComponent&, const glm::mat4&>;
		using PointLightPair	= RenPair<const PointLight&, const Transform&>;
		using DirLightPair		= RenPair<const DirectionalLight&, const Transform&>;

		LightInfo lightInfo;
		GPUCamera camData;

		std::vector<MeshPair> meshes;

		std::vector<PointLightPair> pointLights;
		void expandPointLights();
		void updatePointLightsBuffer();

		std::vector<DirLightPair> dirLights;
		void expandDirLights();
		void updateDirLightsBuffer();


	private: // Buffers
		ID3D11Buffer* pCameraBuffer;
		ID3D11Buffer* pWorldBuffer;
		ID3D11Buffer* pMaterialBuffer;
		ID3D11Buffer* pShaderDataBuffer;

		ID3D11Buffer* pLightInfoBuffer;
		ID3D11Buffer* pPointLightBuffer;
		ID3D11ShaderResourceView* pPointLightSRV;
		ID3D11Buffer* pDirLightBuffer;
		ID3D11ShaderResourceView* pDirLightSRV;

	private: // Pipelines
		void bindNecessities();
		D3D11_VIEWPORT viewport;
		ID3D11RasterizerState* pRSWireFrame;

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