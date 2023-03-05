#include "Renderer.h"
#include "Engine/Okay/Okay.h"

#include "ContentBrowser.h"
#include "RenderTexture.h"

#include "Engine/Components/Transform.h"
#include "Engine/Components/MeshComponent.h"

#include "Engine/Application/Scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <utility>

namespace Okay
{
	std::unique_ptr<Renderer::PipelineResources> Renderer::pipeline;

	void Renderer::init()
	{
		bool result = false;
		HRESULT hr = E_FAIL;

		pipeline = std::make_unique<PipelineResources>();
		DX11& dx11 = DX11::get();
		ID3D11Device* pDevice = dx11.getDevice();
		ID3D11DeviceContext* pDevContext = dx11.getDeviceContext();

		pipeline->skyboxMeshId = ContentBrowser::get().getMeshID("cube");
		if (pipeline->skyboxMeshId == INVALID_UINT)
		{
			bool found = ContentBrowser::get().importFile(ENGINE_RESOURCES_PATH "cube.fbx");
			OKAY_ASSERT(found, "Failed loading cube.fbx");
			pipeline->skyboxMeshId = ContentBrowser::get().getNumMeshes() - 1u;
		}

		// Buffers
		{
			hr = DX11::createConstantBuffer(&pipeline->pMaterialBuffer, nullptr, sizeof(Material::GPUData), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating materialBuffer");

			hr = DX11::createConstantBuffer(&pipeline->pCameraBuffer, nullptr, sizeof(GPUCamera), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating cameraBuffer");

			hr = DX11::createConstantBuffer(&pipeline->pWorldBuffer, nullptr, sizeof(glm::mat4), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating worldBuffer");

			hr = DX11::createConstantBuffer(&pipeline->pLightInfoBuffer, nullptr, sizeof(LightInfo), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating lightInfoBuffer");

			hr = DX11::createConstantBuffer(&pipeline->pShaderDataBuffer, nullptr, sizeof(Shader::GPUData), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating shaderDataBuffer");

			hr = DX11::createConstantBuffer(&pipeline->pSkyDataBuffer, nullptr, sizeof(GPUSkyData), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating skyLightDataBuffer");

			hr = DX11::createConstantBuffer(&pipeline->pSunDataBuffer, nullptr, sizeof(GPUSunData), false);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating sunDataBuffer");




			expandPointLights();
			expandDirLights();
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
			hr = pDevice->CreateRasterizerState(&rsDesc, &pipeline->pWireframeRS);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating wireframeRS");

			rsDesc.FillMode = D3D11_FILL_SOLID;
			hr = pDevice->CreateRasterizerState(&rsDesc, &pipeline->pNoCullRS);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating noCullRS");

		}


		// Basic linear sampler
		{
			ID3D11SamplerState* simp;
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
			hr = pDevice->CreateSamplerState(&simpDesc, &simp);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating sampler");

			pDevContext->PSSetSamplers(0, 1, &simp);
			pDevContext->VSSetSamplers(0, 1, &simp);
			simp->Release();
		}


		// Input Layouts & Shaders
		{
			D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[3] = {
				{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"UV",			0, DXGI_FORMAT_R32G32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			std::string shaderData;
			auto createVSAndInputLayout = [&](std::string_view path, ID3D11VertexShader** ppVS, ID3D11InputLayout** ppIL, uint32_t ilLength)
			{
				result = readBinary(path, shaderData);
				OKAY_ASSERT(result, "Failed reading shader");

				hr = pDevice->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, ppVS);
				OKAY_ASSERT(SUCCEEDED(hr), "Failed creating vertex shader");

				hr = pDevice->CreateInputLayout(inputLayoutDesc, ilLength, shaderData.c_str(), shaderData.length(), ppIL);
				OKAY_ASSERT(SUCCEEDED(hr), "Failed creating input layout");
			};

			createVSAndInputLayout(SHADER_BIN_PATH "MeshVS.cso", &pipeline->pMeshVS, &pipeline->pPosUvNormIL, 3u);


			// Skybox
			{
				createVSAndInputLayout(SHADER_BIN_PATH "SkyBoxVS.cso", &pipeline->pSkyBoxVS, &pipeline->pPosIL, 1u);

				result = readBinary(SHADER_BIN_PATH "SkyBoxPS.cso", shaderData);
				OKAY_ASSERT(result, "Failed reading SkyBoxPS.cso");

				hr = pDevice->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pipeline->pSkyBoxPS);
				OKAY_ASSERT(SUCCEEDED(hr), "Failed creating SkyBoxPS.cso");

			}
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

			hr = pDevice->CreateDepthStencilState(&dsDesc, &pipeline->pLessEqualDSS);
			OKAY_ASSERT(SUCCEEDED(hr), "Failed creating lessEqual Depth Stencil State");
		}

		// Bind necessities
		{
			pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			pDevContext->VSSetConstantBuffers(0, 1, &pipeline->pCameraBuffer);
			pDevContext->VSSetConstantBuffers(1, 1, &pipeline->pWorldBuffer);
			pDevContext->VSSetConstantBuffers(2, 1, &pipeline->pMaterialBuffer);
			pDevContext->VSSetConstantBuffers(3, 1, &pipeline->pShaderDataBuffer);
			pDevContext->VSSetConstantBuffers(4, 1, &pipeline->pSkyDataBuffer);
			pDevContext->VSSetConstantBuffers(5, 1, &pipeline->pSunDataBuffer);
			pDevContext->VSSetConstantBuffers(6, 1, &pipeline->pLightInfoBuffer);

			pDevContext->PSSetConstantBuffers(0, 1, &pipeline->pCameraBuffer);
			pDevContext->PSSetConstantBuffers(1, 1, &pipeline->pWorldBuffer);
			pDevContext->PSSetConstantBuffers(2, 1, &pipeline->pMaterialBuffer);
			pDevContext->PSSetConstantBuffers(3, 1, &pipeline->pShaderDataBuffer);
			pDevContext->PSSetConstantBuffers(4, 1, &pipeline->pSkyDataBuffer);
			pDevContext->PSSetConstantBuffers(5, 1, &pipeline->pSunDataBuffer);
			pDevContext->PSSetConstantBuffers(6, 1, &pipeline->pLightInfoBuffer);

			// 0-2	| material textures
			// 3	| height map 
			pDevContext->PSSetShaderResources(4, 1, &pipeline->pPointLightSRV);
			pDevContext->PSSetShaderResources(5, 1, &pipeline->pDirLightSRV);
			// 6	| skyBoxTextureCube
		}

	}

	Renderer::Renderer(RenderTexture* pRenderTarget)
		:pDevContext(DX11::get().getDeviceContext()), pRenderTarget(pRenderTarget)
	{
		OKAY_ASSERT(pRenderTarget, "RenderTarget was nullptr");

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
		if (pointLights.size() >= (uint32_t)pipeline->maxPointLights)
			expandPointLights();

		pointLights.emplace_back(light, transform);
		lightInfo.numPointLights++;
	}

	void Renderer::submit(const DirectionalLight& light, const Transform& transform)
	{
		if (dirLights.size() >= (uint32_t)pipeline->maxDirLights)
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
}

#include "imgui/imgui.h"
#include <d3dcompiler.h>
namespace Okay 
{
	void Renderer::imGui()
	{
		ImGui::Begin("Skybox stuff");

		if (ImGui::Button("Refresh"))
		{
			ID3DBlob* outData = nullptr;
			ID3DBlob* outErrors = nullptr;

			static const wchar_t* path = L"../OkayEngine/source/Engine/Graphics/Shaders/SkyBoxPS.hlsl";
			static const size_t pathSize = wcslen(path);

			HRESULT hr = D3DCompileFromFile(path, nullptr, nullptr, "main", "ps_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL2, 0u, &outData, &outErrors);

			ID3D11PixelShader* pNewPS = nullptr;
			if (SUCCEEDED(hr))
			{
				DX11::get().getDevice()->CreatePixelShader(outData->GetBufferPointer(), outData->GetBufferSize(), nullptr, &pNewPS);
				DX11_RELEASE(outData);
				if (!pNewPS)
				{
					ImGui::End();
					return;
				}
			}
			else
			{
				if (outErrors)
				{
					printf("Error compiling shader: %s\n", (char*)outErrors->GetBufferPointer());
					DX11_RELEASE(outErrors);
				}
				ImGui::End();
				return;
			}

			DX11_RELEASE(pipeline->pSkyBoxPS);
			pipeline->pSkyBoxPS = pNewPS;
		}

		ImGui::End();
	}

	void Renderer::render(const Entity& camera)
	{
		Entity actualCamera = camera ? camera : pScene->getMainCamera();

		// Should only occur if a scene is started without a camera
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

		pScene->submit(this);
		render_internal();
	}

	void Renderer::render_internal()
	{
		ContentBrowser& content = ContentBrowser::get();
		ID3D11ShaderResourceView* textures[3] = {};
		glm::mat4 worldMatrix{};

		const Entity skyLightEntity = pScene->getSkyLight();
		const Entity sunEntity = pScene->getSun();

		//TODO: Display warning if nullptr 
		const SkyLight* pSkyLight = skyLightEntity ? skyLightEntity.tryGetComponent<SkyLight>() : nullptr;
		const Sun* pSun = sunEntity ? sunEntity.tryGetComponent<Sun>() : nullptr;

		if (pSkyLight)
		{
			DX11::updateBuffer(pipeline->pSkyDataBuffer, &pSkyLight->tint, sizeof(GPUSkyData));
		}
		else
		{
			GPUSkyData data;
			DX11::updateBuffer(pipeline->pSkyDataBuffer, &data, sizeof(GPUSkyData));
		}

		if (pSun)
		{
			GPUSunData data;
			data.direction = -sunEntity.getComponent<Transform>().forward();
			data.colour = pSun->colour;
			data.size = pSun->size;
			data.intensity = pSun->intensity;
			DX11::updateBuffer(pipeline->pSunDataBuffer, &data, sizeof(GPUSunData));
		}
		else
		{
			GPUSunData data;
			DX11::updateBuffer(pipeline->pSunDataBuffer, &data, sizeof(GPUSunData));
		}

		updatePointLightsBuffer();
		updateDirLightsBuffer();
		DX11::updateBuffer(pipeline->pLightInfoBuffer, &lightInfo, (uint32_t)sizeof(LightInfo));

		bindMeshPipeline();
		pDevContext->OMSetRenderTargets(1u, pRenderTarget->getRTV(), *pRenderTarget->getDSV());

		// TODO: Add wireFrame as a material property aswell
		pDevContext->RSSetState(pWireframeRS);

		// Draw all static meshes
		for (size_t i = 0; i < meshes.size(); i++)
		{
			const MeshComponent& cMesh = meshes[i].asset;
			const Mesh& mesh = content.getMesh(cMesh.meshIdx);
			const Material& material = content.getMaterial(cMesh.materialIdx);
			const Shader& shader = content.getShader(cMesh.shaderIdx);

			worldMatrix = glm::transpose(meshes[i].transform);

			textures[Material::BASECOLOUR_INDEX] = content.getTexture(material.getBaseColour()).getSRV();
			textures[Material::SPECULAR_INDEX] = content.getTexture(material.getSpecular()).getSRV();
			textures[Material::AMBIENT_INDEX] = content.getTexture(material.getAmbient()).getSRV();

			DX11::updateBuffer(pipeline->pWorldBuffer, &worldMatrix, sizeof(glm::mat4));
			DX11::updateBuffer(pipeline->pMaterialBuffer, &material.getGPUData(), sizeof(Material::GPUData));
			DX11::updateBuffer(pipeline->pShaderDataBuffer, &shader.getGPUData(), sizeof(Shader::GPUData));

			// IA
			pDevContext->IASetVertexBuffers(0u, Mesh::NumBuffers, mesh.getBuffers(), Mesh::Stride, Mesh::Offset);
			pDevContext->IASetIndexBuffer(mesh.getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0u);

			// PS
			shader.bind();
			pDevContext->PSSetShaderResources(0u, 3u, textures);

			// Draw
			pDevContext->DrawIndexed(mesh.getNumIndices(), 0u, 0);
		}

		//TODO: Create constants for d3d11 resource slots

		// Skybox
		if (pSkyLight)
		{
			const Mesh& skyBoxMesh = content.getMesh(pipeline->skyboxMeshId);

			pDevContext->IASetInputLayout(pipeline->pPosIL);

			pDevContext->IASetVertexBuffers(0u, Mesh::NumBuffers, skyBoxMesh.getBuffers(), Mesh::Stride, Mesh::Offset);
			pDevContext->IASetIndexBuffer(skyBoxMesh.getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0u);

			pDevContext->VSSetShader(pipeline->pSkyBoxVS, nullptr, 0u);

			pDevContext->RSSetState(pipeline->pNoCullRS);

			pDevContext->PSSetShader(pipeline->pSkyBoxPS, nullptr, 0u);
			pDevContext->PSSetShaderResources(6u, 1, pSkyLight->skyBox->getTextureCubeSRV());

			pDevContext->OMSetDepthStencilState(pipeline->pLessEqualDSS, 0u);

			pDevContext->DrawIndexed(skyBoxMesh.getNumIndices(), 0u, 0);
		}
	}

	void Renderer::onTargetResize(uint32_t width, uint32_t height)
	{
		viewport.Width = (float)width;
		viewport.Height = (float)height;
		pDevContext->RSSetViewports(1u, &viewport);
	}

	void Renderer::newFrame()
	{
		lightInfo.numPointLights = 0u;
		lightInfo.numDirLights = 0u;
		meshes.clear();
		pointLights.clear();
		dirLights.clear();
	}

	void Renderer::setWireframe(bool wireFrame)
	{
		if (wireFrame)
		{
			pWireframeRS = pipeline->pWireframeRS;
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

		DX11_RELEASE(pipeline->pPointLightBuffer);
		DX11_RELEASE(pipeline->pPointLightSRV);

		pipeline->maxPointLights += 5u;

		hr = DX11::createStructuredBuffer(&pipeline->pPointLightBuffer, nullptr, GPUPointLightSize, pipeline->maxPointLights, false);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating pointLight structured buffer");

		hr = DX11::createStructuredSRV(&pipeline->pPointLightSRV, pipeline->pPointLightBuffer, pipeline->maxPointLights);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating pointLight SRV");

		DX11::get().getDeviceContext()->PSSetShaderResources(3, 1, &pipeline->pPointLightSRV);
	}

	void Renderer::updatePointLightsBuffer()
	{
		D3D11_MAPPED_SUBRESOURCE sub;
		if (FAILED(pDevContext->Map(pipeline->pPointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub)))
			return;

		char* location = (char*)sub.pData;
		for (size_t i = 0; i < pointLights.size(); i++)
		{
			memcpy(location, &pointLights[i].asset, sizeof(PointLight));
			location += sizeof(PointLight);

			memcpy(location, &pointLights[i].transform.position, sizeof(glm::vec3));
			location += sizeof(glm::vec3);
		}

		pDevContext->Unmap(pipeline->pPointLightBuffer, 0);
	}

	void Renderer::expandDirLights()
	{;
		static const size_t GPUDirLightSize = sizeof(DirectionalLight) + sizeof(glm::vec3);
		HRESULT hr = E_FAIL;

		DX11_RELEASE(pipeline->pDirLightBuffer);
		DX11_RELEASE(pipeline->pDirLightSRV);

		pipeline->maxDirLights += 5u;

		hr = DX11::createStructuredBuffer(&pipeline->pDirLightBuffer, nullptr, GPUDirLightSize, pipeline->maxDirLights, false);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating dirLight structured buffer");

		hr = DX11::createStructuredSRV(&pipeline->pDirLightSRV, pipeline->pDirLightBuffer, pipeline->maxDirLights);
		OKAY_ASSERT(SUCCEEDED(hr), "Failed recreating dirLight SRV");

		DX11::get().getDeviceContext()->PSSetShaderResources(4, 1, &pipeline->pDirLightSRV);
	}

	void Renderer::updateDirLightsBuffer()
	{
		D3D11_MAPPED_SUBRESOURCE sub;
		if (FAILED(pDevContext->Map(pipeline->pDirLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub)))
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

		pDevContext->Unmap(pipeline->pDirLightBuffer, 0);
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

		DX11::updateBuffer(pipeline->pCameraBuffer, &camData, sizeof(GPUCamera));
	}

	void Renderer::bindMeshPipeline()
	{
		ID3D11DeviceContext* pDevContext = DX11::get().getDeviceContext();
		pDevContext->IASetInputLayout(pipeline->pPosUvNormIL);
		pDevContext->VSSetShader(pipeline->pMeshVS, nullptr, 0u);
		pDevContext->OMSetDepthStencilState(nullptr, 0u);
	}

	void Renderer::bindSkeletalPipeline()
	{
		return;
		DX11::get().getDeviceContext()->IASetInputLayout(pipeline->pPosUvNormJidxWeightIL);
		DX11::get().getDeviceContext()->VSSetShader(pipeline->pSkeletalVS, nullptr, 0);
	}
	
	Renderer::PipelineResources::~PipelineResources()
	{
		DX11_RELEASE(pCameraBuffer);
		DX11_RELEASE(pWorldBuffer);
		DX11_RELEASE(pMaterialBuffer);
		DX11_RELEASE(pShaderDataBuffer);

		DX11_RELEASE(pSkyDataBuffer);
		DX11_RELEASE(pSunDataBuffer);

		DX11_RELEASE(pLightInfoBuffer);
		DX11_RELEASE(pPointLightBuffer);
		DX11_RELEASE(pDirLightBuffer);
		DX11_RELEASE(pPointLightSRV);
		DX11_RELEASE(pDirLightSRV);

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
