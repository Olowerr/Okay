#include "Renderer.h"
#include "Engine/Okay/Okay.h"

#include "ContentBrowser.h"
#include "RenderTexture.h"

#include "Engine/Components/Transform.h"
#include "Engine/Components/MeshComponent.h"

#include "Engine/Application/Scene.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <utility>

namespace Okay
{
	Renderer::PipelineResources Renderer::pipeline;

	void Renderer::init()
	{
		bool result = false;
		HRESULT hr = E_FAIL;
		DX11& dx11 = DX11::get();
		ID3D11Device* pDevice = dx11.getDevice();
		ContentBrowser& content = ContentBrowser::get();

		content.addAsset<Shader>("Phong", SHADER_PATH "PhongPS.hlsl", true);
		pipeline.defaultShaderId = content.getAmount<Shader>() - 1u;

		content.addAsset<Material>(Okay::Material::Description()).setName("Default");
		content.importFile(ENGINE_RESOURCES_PATH "textures/DefaultTexture.png");

		pipeline.skyboxMeshId = content.getAssetID<Mesh>("cube");
		if (pipeline.skyboxMeshId == INVALID_UINT)
		{
			bool found = content.importFile(ENGINE_RESOURCES_PATH "meshes/cube.fbx");
			OKAY_ASSERT(found, "Failed loading cube.fbx");
			pipeline.skyboxMeshId = content.getAmount<Mesh>() - 1u;
		}

		// Buffers
		{
			hr = DX11::createConstantBuffer(&pipeline.pMaterialBuffer, nullptr, sizeof(Material::GPUData), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating materialBuffer");

			hr = DX11::createConstantBuffer(&pipeline.pCameraBuffer, nullptr, sizeof(GPUCamera), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating cameraBuffer");

			hr = DX11::createConstantBuffer(&pipeline.pWorldBuffer, nullptr, sizeof(glm::mat4), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating worldBuffer");

			hr = DX11::createConstantBuffer(&pipeline.pLightInfoBuffer, nullptr, sizeof(LightInfo), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating lightInfoBuffer");

			hr = DX11::createConstantBuffer(&pipeline.pShaderDataBuffer, nullptr, sizeof(Shader::GPUData), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating shaderDataBuffer");

			hr = DX11::createConstantBuffer(&pipeline.pSkyDataBuffer, nullptr, sizeof(GPUSkyData), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating skyLightDataBuffer");
		}

		// Rasterizer states
		{
			D3D11_RASTERIZER_DESC rsDesc{};
			rsDesc.FillMode = D3D11_FILL_WIREFRAME;
			rsDesc.CullMode = D3D11_CULL_NONE;
			rsDesc.FrontCounterClockwise = FALSE;
			rsDesc.DepthBias = 0;
			rsDesc.SlopeScaledDepthBias = 0.0f;
			rsDesc.DepthBiasClamp = 0.0f;
			rsDesc.DepthClipEnable = TRUE;
			rsDesc.ScissorEnable = FALSE;
			rsDesc.MultisampleEnable = FALSE;
			rsDesc.AntialiasedLineEnable = FALSE;
			hr = pDevice->CreateRasterizerState(&rsDesc, &pipeline.pWireframeRS);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating wireframeRS");

			rsDesc.FillMode = D3D11_FILL_SOLID;
			hr = pDevice->CreateRasterizerState(&rsDesc, &pipeline.pNoCullRS);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating noCullRS");

		}


		// Basic linear sampler
		{
			D3D11_SAMPLER_DESC simpDesc{};
			simpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			simpDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			simpDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			simpDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			simpDesc.MinLOD = -FLT_MAX;
			simpDesc.MaxLOD = FLT_MAX;
			simpDesc.MipLODBias = 0.f;
			simpDesc.MaxAnisotropy = 1u;
			simpDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			hr = pDevice->CreateSamplerState(&simpDesc, &pipeline.simp);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating sampler");
		}


		// Input Layouts & Shaders
		{
			D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[3] = {
				{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"UV",			0, DXGI_FORMAT_R32G32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			std::string shaderData;
			auto createVSAndInputLayout = [&](std::string_view path, ID3D11VertexShader** ppVS, ID3D11InputLayout** ppIL, uint32_t numILElements)
			{
				bool result = DX11::createShader(path, ppVS, &shaderData);
				OKAY_ASSERT(result, "Failed creating vertex shader");

				hr = pDevice->CreateInputLayout(inputLayoutDesc, numILElements, shaderData.c_str(), shaderData.length(), ppIL);
				OKAY_ASSERT(SUCCEEDED(hr), "Failed creating input layout");
			};


			createVSAndInputLayout(SHADER_PATH "MeshVS.hlsl", &pipeline.pMeshVS, &pipeline.pPosUvNormIL, 3u);

			// Skybox
			createVSAndInputLayout(SHADER_PATH "SkyBoxVS.hlsl", &pipeline.pSkyBoxVS, &pipeline.pPosIL, 1u);
			DX11::createShader(SHADER_PATH "SkyBoxPS.hlsl", &pipeline.pSkyBoxPS);


#if 0 // Skeletal Animation (OLD)
			D3D11_INPUT_ELEMENT_DESC aniDesc[5] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"JOINTIDX", 0, DXGI_FORMAT_R32G32B32A32_UINT,  1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"WEIGHTS",	 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"UV",		 0, DXGI_FORMAT_R32G32_FLOAT,		2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,	3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			result = Shader::readShader("SkeletalMeshVS.cso", shaderData);
			OKAY_ASSERT(result, "Failed reading SkeletalMeshVS.cso");

			hr = dx11.getDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &pAniVS);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating skeletal mesh Vertex Shader");

			hr = dx11.getDevice()->CreateInputLayout(desc, 3, shaderData.c_str(), shaderData.length(), &pAniIL);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating skeletal mesh Input Layout");
#endif
		}

		// Depthstenil states
		{
			D3D11_DEPTH_STENCIL_DESC dsDesc;
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Comparison_Equal no bueno (?)
			dsDesc.StencilEnable = false;
			dsDesc.StencilReadMask = 0;
			dsDesc.StencilWriteMask = 0;

			hr = pDevice->CreateDepthStencilState(&dsDesc, &pipeline.pLessEqualDSS);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating lessEqual Depth Stencil State");
		}

#if 0
		// Bind necessities
		{
			pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			pDevContext->VSSetConstantBuffers(0, 1, &pipeline.pCameraBuffer);
			pDevContext->VSSetConstantBuffers(1, 1, &pipeline.pWorldBuffer);
			pDevContext->VSSetConstantBuffers(2, 1, &pipeline.pMaterialBuffer);
			pDevContext->VSSetConstantBuffers(3, 1, &pipeline.pShaderDataBuffer);
			pDevContext->VSSetConstantBuffers(4, 1, &pipeline.pSkyDataBuffer);
			pDevContext->VSSetConstantBuffers(5, 1, &pipeline.pLightInfoBuffer);

			pDevContext->PSSetConstantBuffers(0, 1, &pipeline.pCameraBuffer);
			pDevContext->PSSetConstantBuffers(1, 1, &pipeline.pWorldBuffer);
			pDevContext->PSSetConstantBuffers(2, 1, &pipeline.pMaterialBuffer);
			pDevContext->PSSetConstantBuffers(3, 1, &pipeline.pShaderDataBuffer);
			pDevContext->PSSetConstantBuffers(4, 1, &pipeline.pSkyDataBuffer);
			pDevContext->PSSetConstantBuffers(5, 1, &pipeline.pLightInfoBuffer);

			// 0-1	| material textures
			// 2	| height map 
			pDevContext->PSSetShaderResources(3, 1, &pipeline.pPointLightSRV);
			pDevContext->PSSetShaderResources(4, 1, &pipeline.pDirLightSRV);
			// 5	| skyBoxTextureCube
		}
#endif
	}

	Renderer::Renderer(RenderTexture* pRenderTarget, Scene* scene)
		:pRenderTarget(pRenderTarget), pScene(scene)
	{
		OKAY_ASSERT(pRenderTarget, "RenderTarget was nullptr");
		OKAY_ASSERT(pScene, "Scene was nullptr");

#ifdef MULTI_THREADED
		HRESULT hr = DX11::get().getDevice()->CreateDeferredContext(0u, &pDefContext);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed creating DeferredContext");
#else
		pDefContext = DX11::get().getDeviceContext();
#endif
		expandPointLights();
		expandDirLights();

		meshes.reserve(10);
		pointLights.reserve(10);
		dirLights.reserve(2);

		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		glm::ivec2 dims = pRenderTarget->getDimensions();
		onTargetResize((uint32_t)dims.x, (uint32_t)dims.y);
	}

	Renderer::~Renderer()
	{
		DX11_RELEASE(pWireframeRS);
		DX11_RELEASE(pCommandList);

#ifdef MULTI_THREADED
		DX11_RELEASE(pDefContext);
#endif
	}

	void Renderer::submit(const MeshComponent& mesh, const Transform& transform)
	{
		// Remove? std::vector reallocates itself but having control over the amount can be important
		if (meshes.size() == meshes.capacity())
			meshes.reserve(meshes.size() + 10ull);

		meshes.emplace_back(mesh, transform.matrix);
	}

	void Renderer::submit(const PointLight& light, const Transform& transform)
	{
		if ((uint32_t)pointLights.size() >= pipeline.maxPointLights)
			expandPointLights();

		pointLights.emplace_back(light, transform);
		lightInfo.numPointLights++;
	}

	void Renderer::submit(const DirectionalLight& light, const Transform& transform)
	{
		if ((uint32_t)dirLights.size() >= pipeline.maxDirLights)
			expandDirLights();

		dirLights.emplace_back(light, transform);
		lightInfo.numDirLights++;
	}

	void Renderer::setRenderTexture(RenderTexture* pRenderTexture)
	{
		OKAY_ASSERT(pRenderTexture, "RenderTarget was nullptr");

		pRenderTarget->removeOnResizeCallback(&Renderer::onTargetResize, this);
		pRenderTarget = pRenderTexture;
		pRenderTarget->addOnResizeCallback(&Renderer::onTargetResize, this);

		glm::ivec2 dims = pRenderTarget->getDimensions();
		onTargetResize((uint32_t)dims.x, (uint32_t)dims.y);
	}

#define VS_UPDATE_BUTTON(shader, pVS)\
if (ImGui::Button(shader))\
{\
ID3D11VertexShader* pNewVS = nullptr;\
DX11::createShader(SHADER_PATH shader, &pNewVS);\
if (!pNewVS)\
	return;\
DX11_RELEASE(pVS);\
pVS = pNewVS;\
}\

#define PS_UPDATE_BUTTON(shader, pPS)\
if (ImGui::Button(shader))\
{\
ID3D11PixelShader* pNewPS = nullptr;\
DX11::createShader(SHADER_PATH shader ".hlsl", &pNewPS);\
if (!pNewPS)\
	return;\
DX11_RELEASE(pPS);\
pPS = pNewPS;\
}\

	void Renderer::imGui()
	{
		if (!ImGui::Begin("Shaders"))
		{
			ImGui::End();
			return;
		}

		ImGui::Text("Reload shaders:");
		VS_UPDATE_BUTTON("MeshVS", pipeline.pMeshVS);

		VS_UPDATE_BUTTON("SkyBoxVS", pipeline.pSkyBoxVS);
		PS_UPDATE_BUTTON("SkyBoxPS", pipeline.pSkyBoxPS);


		ImGui::End();
	}

	void Renderer::prepareForRecording()
	{
		lightInfo.numPointLights = 0u;
		lightInfo.numDirLights = 0u;
		meshes.clear();
		pointLights.clear();
		dirLights.clear();

		static const float clearColour[4]{ 1.f, 1.f, 1.f, 1.f };
		pDefContext->ClearRenderTargetView(*pRenderTarget->getRTV(), clearColour);
		if (ID3D11DepthStencilView* pDsv = *pRenderTarget->getDSV())
			pDefContext->ClearDepthStencilView(pDsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		pScene->submit(this);
	}

	void Renderer::executeCommands()
	{
#ifdef MULTI_THREADED
		if (!pCommandList)
			return;

		DX11::get().getDeviceContext()->ExecuteCommandList(pCommandList, FALSE);
		DX11_RELEASE(pCommandList);
#endif
	}

	void Renderer::recordCommands()
	{
		Entity actualCamera = customCamera ? customCamera : pScene->getMainCamera();

		// Should only occur if a there's no customCamera and no mainCamera in the scene
		// Maybe add a OKAY_DEBUG/OKAY_SAFE define?
		bool bad = false;
		if (!actualCamera) 
		{
			actualCamera = pScene->createEntity();
			actualCamera.addComponent<Camera>();
			bad = true;
		}

		updateCameraBuffer(actualCamera);
		if (bad)
			pScene->destroyEntity(actualCamera);

		render_internal();
#ifdef MULTI_THREADED
		pDefContext->FinishCommandList(FALSE, &pCommandList);
#endif
	}

	void Renderer::render_internal()
	{
		ContentBrowser& content = ContentBrowser::get();
		ID3D11ShaderResourceView* textures[2] = {};
		glm::mat4 worldMatrix{};

		const Entity skyLightEntity = pScene->getSkyLight();

		//TODO: Display warning if nullptr 
		const SkyLight* pSkyLight = skyLightEntity ? skyLightEntity.tryGetComponent<SkyLight>() : nullptr;

		GPUSkyData skyData;	
		if (pSkyLight)
		{
			skyData.ambientTint = pSkyLight->ambientTint;
			skyData.ambientTintIntensity = pSkyLight->ambientIntensity;

			skyData.sunDirection = -skyLightEntity.getComponent<Transform>().forward();
			skyData.sunColour = pSkyLight->sunColour;
			skyData.sunSize = pSkyLight->sunSize;
			skyData.sunIntensity = pSkyLight->sunIntensity;
		}
		DX11::updateBuffer(pipeline.pSkyDataBuffer, &skyData, sizeof(GPUSkyData), pDefContext);

		updatePointLightsBuffer();
		updateDirLightsBuffer();
		DX11::updateBuffer(pipeline.pLightInfoBuffer, &lightInfo, (uint32_t)sizeof(LightInfo), pDefContext);
		
		bind();

		// Draw all static meshes
		bindMeshPipeline();
		for (size_t i = 0; i < meshes.size(); i++)
		{
			const MeshComponent& cMesh = meshes[i].asset;
			const Mesh& mesh = content.getAsset<Mesh>(cMesh.meshIdx);
			const Material& material = content.getAsset<Material>(cMesh.materialIdx);
			const Shader& shader = content.getAsset<Shader>(cMesh.shaderIdx);

			worldMatrix = glm::transpose(meshes[i].transform);

			textures[Material::BASECOLOUR_INDEX] = content.getAsset<Texture>(material.getBaseColour()).getSRV();
			textures[Material::SPECULAR_INDEX] = content.getAsset<Texture>(material.getSpecular()).getSRV();

			DX11::updateBuffer(pipeline.pWorldBuffer, &worldMatrix, sizeof(glm::mat4), pDefContext);
			DX11::updateBuffer(pipeline.pMaterialBuffer, &material.getGPUData(), sizeof(Material::GPUData), pDefContext);
			DX11::updateBuffer(pipeline.pShaderDataBuffer, &shader.getGPUData(), sizeof(Shader::GPUData), pDefContext);

			// IA
			pDefContext->IASetVertexBuffers(0u, Mesh::NumBuffers, mesh.getBuffers(), Mesh::Stride, Mesh::Offset);
			pDefContext->IASetIndexBuffer(mesh.getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0u);

			// PS
			shader.bind(pDefContext);
			pDefContext->PSSetShaderResources(0u, 2u, textures);

			// Draw
			pDefContext->DrawIndexed(mesh.getNumIndices(), 0u, 0);
		}

		//TODO: Create constants for d3d11 resource slots

		// Skybox
		if (pSkyLight)
		{
			const Mesh& skyBoxMesh = content.getAsset<Mesh>(pipeline.skyboxMeshId);

			pDefContext->IASetInputLayout(pipeline.pPosIL);

			pDefContext->IASetVertexBuffers(0u, Mesh::NumBuffers, skyBoxMesh.getBuffers(), Mesh::Stride, Mesh::Offset);
			pDefContext->IASetIndexBuffer(skyBoxMesh.getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0u);

			pDefContext->VSSetShader(pipeline.pSkyBoxVS, nullptr, 0u);

			pDefContext->RSSetState(pipeline.pNoCullRS);

			pDefContext->PSSetShader(pipeline.pSkyBoxPS, nullptr, 0u);
			pDefContext->PSSetShaderResources(5u, 1u, pSkyLight->skyBox->getTextureCubeSRV());

			pDefContext->OMSetDepthStencilState(pipeline.pLessEqualDSS, 0u);

			pDefContext->DrawIndexed(skyBoxMesh.getNumIndices(), 0u, 0);
		}
	}

	void Renderer::onTargetResize(uint32_t width, uint32_t height)
	{
		viewport.Width = (float)width;
		viewport.Height = (float)height;
		pDefContext->RSSetViewports(1u, &viewport);
	}

	void Renderer::setWireframe(bool wireFrame)
	{
		if (wireFrame)
		{
			pWireframeRS = pipeline.pWireframeRS;
			pWireframeRS->AddRef();
		}
		else
		{
			DX11_RELEASE(pWireframeRS);
		}
	}

	void Renderer::expandPointLights()
	{
		static const size_t GPUPointLightSize = sizeof(PointLight) + sizeof(glm::vec3);
		HRESULT hr = E_FAIL;

		DX11_RELEASE(pipeline.pPointLightBuffer);
		DX11_RELEASE(pipeline.pPointLightSRV);

		pipeline.maxPointLights += 5u;

		hr = DX11::createStructuredBuffer(&pipeline.pPointLightBuffer, nullptr, GPUPointLightSize, pipeline.maxPointLights, false);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating pointLight structured buffer");

		hr = DX11::createStructuredSRV(&pipeline.pPointLightSRV, pipeline.pPointLightBuffer, pipeline.maxPointLights);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating pointLight SRV");

		pDefContext->PSSetShaderResources(3, 1, &pipeline.pPointLightSRV);
	}

	void Renderer::updatePointLightsBuffer()
	{
		D3D11_MAPPED_SUBRESOURCE sub;
		if (FAILED(pDefContext->Map(pipeline.pPointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub)))
			return;

		char* location = (char*)sub.pData;
		for (size_t i = 0; i < pointLights.size(); i++)
		{
			memcpy(location, &pointLights[i].asset, sizeof(PointLight));
			location += sizeof(PointLight);

			memcpy(location, &pointLights[i].transform.position, sizeof(glm::vec3));
			location += sizeof(glm::vec3);
		}

		pDefContext->Unmap(pipeline.pPointLightBuffer, 0);
	}

	void Renderer::expandDirLights()
	{;
		static const size_t GPUDirLightSize = sizeof(DirectionalLight) + sizeof(glm::vec3);
		HRESULT hr = E_FAIL;

		DX11_RELEASE(pipeline.pDirLightBuffer);
		DX11_RELEASE(pipeline.pDirLightSRV);

		pipeline.maxDirLights += 5u;

		hr = DX11::createStructuredBuffer(&pipeline.pDirLightBuffer, nullptr, GPUDirLightSize, pipeline.maxDirLights, false);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating dirLight structured buffer");

		hr = DX11::createStructuredSRV(&pipeline.pDirLightSRV, pipeline.pDirLightBuffer, pipeline.maxDirLights);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating dirLight SRV");

		pDefContext->PSSetShaderResources(4, 1, &pipeline.pDirLightSRV);
	}

	void Renderer::updateDirLightsBuffer()
	{
		D3D11_MAPPED_SUBRESOURCE sub;
		if (FAILED(pDefContext->Map(pipeline.pDirLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub)))
			return;

		glm::vec3 dirLightDirection{};
		char* location = (char*)sub.pData;
		for (size_t i = 0; i < dirLights.size(); i++)
		{
			dirLightDirection = dirLights[i].transform.forward();

			memcpy(location, &dirLights[i].asset, sizeof(DirectionalLight));
			location += sizeof(DirectionalLight);

			memcpy(location, &dirLightDirection, sizeof(glm::vec3));
			location += sizeof(glm::vec3);
		}

		pDefContext->Unmap(pipeline.pDirLightBuffer, 0);
	}

	void Renderer::updateCameraBuffer(const Entity& cameraEntity)
	{
		const Camera& camera = cameraEntity.getComponent<Camera>();
		const Transform& camTransform = cameraEntity.getComponent<Okay::Transform>();

		GPUCamera camData{};
		camData.direction = camTransform.forward();
		camData.pos = camTransform.position;
		camData.viewProjMatrix = glm::transpose(camera.projectionMatrix *
			glm::lookAtLH(camTransform.position, camTransform.position + camData.direction, camTransform.up()));

		DX11::updateBuffer(pipeline.pCameraBuffer, &camData, sizeof(GPUCamera), pDefContext);
	}

	void Renderer::bind()
	{
		pDefContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDefContext->VSSetConstantBuffers(0, 1, &pipeline.pCameraBuffer);
		pDefContext->VSSetConstantBuffers(1, 1, &pipeline.pWorldBuffer);
		pDefContext->VSSetConstantBuffers(2, 1, &pipeline.pMaterialBuffer);
		pDefContext->VSSetConstantBuffers(3, 1, &pipeline.pShaderDataBuffer);
		pDefContext->VSSetConstantBuffers(4, 1, &pipeline.pSkyDataBuffer);
		pDefContext->VSSetConstantBuffers(5, 1, &pipeline.pLightInfoBuffer);

		pDefContext->PSSetConstantBuffers(0, 1, &pipeline.pCameraBuffer);
		pDefContext->PSSetConstantBuffers(1, 1, &pipeline.pWorldBuffer);
		pDefContext->PSSetConstantBuffers(2, 1, &pipeline.pMaterialBuffer);
		pDefContext->PSSetConstantBuffers(3, 1, &pipeline.pShaderDataBuffer);
		pDefContext->PSSetConstantBuffers(4, 1, &pipeline.pSkyDataBuffer);
		pDefContext->PSSetConstantBuffers(5, 1, &pipeline.pLightInfoBuffer);
		pDefContext->PSSetShaderResources(3, 1, &pipeline.pPointLightSRV);
		pDefContext->PSSetShaderResources(4, 1, &pipeline.pDirLightSRV);

		pDefContext->VSSetSamplers(0u, 1u, &pipeline.simp);
		pDefContext->PSSetSamplers(0u, 1u, &pipeline.simp);

		pDefContext->RSSetState(pWireframeRS);
		pDefContext->RSSetViewports(1u, &viewport);

		pDefContext->OMSetRenderTargets(1u, pRenderTarget->getRTV(), *pRenderTarget->getDSV());

	}

	void Renderer::bindMeshPipeline()
	{
		pDefContext->IASetInputLayout(pipeline.pPosUvNormIL);
		pDefContext->VSSetShader(pipeline.pMeshVS, nullptr, 0u);
		pDefContext->OMSetDepthStencilState(nullptr, 0u);
	}

	void Renderer::bindSkeletalPipeline()
	{
		return;
		DX11::get().getDeviceContext()->IASetInputLayout(pipeline.pPosUvNormJidxWeightIL);
		DX11::get().getDeviceContext()->VSSetShader(pipeline.pSkeletalVS, nullptr, 0);
	}
	
	Renderer::PipelineResources::~PipelineResources()
	{
		DX11_RELEASE(pCameraBuffer);
		DX11_RELEASE(pWorldBuffer);
		DX11_RELEASE(pMaterialBuffer);
		DX11_RELEASE(pShaderDataBuffer);

		DX11_RELEASE(pSkyDataBuffer);

		DX11_RELEASE(pLightInfoBuffer);
		DX11_RELEASE(pPointLightBuffer);
		DX11_RELEASE(pDirLightBuffer);
		DX11_RELEASE(pPointLightSRV);
		DX11_RELEASE(pDirLightSRV);

		DX11_RELEASE(simp);

		DX11_RELEASE(pPosIL);
		DX11_RELEASE(pPosUvNormIL);
		DX11_RELEASE(pPosUvNormJidxWeightIL);

		DX11_RELEASE(pMeshVS);
		DX11_RELEASE(pSkeletalVS);
		DX11_RELEASE(pSkyBoxVS);

		DX11_RELEASE(pWireframeRS);
		DX11_RELEASE(pNoCullRS);

		DX11_RELEASE(pSkyBoxPS);

		DX11_RELEASE(pLessEqualDSS);
	}
}
