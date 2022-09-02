#include "Renderer.h"
#include "Engine.h"

Renderer::Renderer()
	:pInputLayout(), pVertexShader(), pHullShader(), pDomainShader(), pDevContext(DX11::Get().GetDeviceContext())
{
	// Make sure Okay::Engine::Get() is never called here

	shaderModel = std::make_unique<Okay::ShaderModel>(true);
	mainCamera = std::make_unique<Okay::Camera>();

	DirectX::XMFLOAT4X4 Identity4x4;
	DirectX::XMStoreFloat4x4(&Identity4x4, DirectX::XMMatrixIdentity());

	DX11::CreateConstantBuffer(&pMaterialBuffer, nullptr, sizeof(Okay::MaterialGPUData), false);
	DX11::CreateConstantBuffer(&pViewProjectBuffer, &mainCamera->GetViewProjectMatrix(), sizeof(DirectX::XMFLOAT4X4), false);
	DX11::CreateConstantBuffer(&pWorldBuffer, &Identity4x4, sizeof(DirectX::XMFLOAT4X4), false);
	DX11::CreateConstantBuffer(&pLightInfoBuffer, nullptr, 16, false);

	CreateVS();
	CreateHS();
	CreateDS();
	{
		ID3D11SamplerState* simp;
		D3D11_SAMPLER_DESC desc;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.MipLODBias = 0.f;
		desc.MaxAnisotropy = 1U;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		DX11::Get().GetDevice()->CreateSamplerState(&desc, &simp);

		pDevContext->PSSetSamplers(0, 1, &simp);
		simp->Release();
	}

	Bind();
	shaderModel->Bind();

	meshesToRender.resize(10);
	numActive = 0;
	numLights = 0;

#if ANIMATION == 1
	CreateSkeletal();
	DX11::CreateStructuredBuffer<DirectX::XMFLOAT4X4A>(&aniBuffer, nullptr, (UINT)joints.size(), false);
	DX11::CreateStructuredSRV<DirectX::XMFLOAT4X4A>(&aniSRV, aniBuffer, (UINT)joints.size());
#endif
}

Renderer::~Renderer()
{
	Shutdown();
}

void Renderer::Resize()
{
	shaderModel->Resize();
}

void Renderer::Submit(Okay::CompMesh* pMesh, Okay::CompTransform* pTransform)
{
	if (numActive >= meshesToRender.size())
		meshesToRender.resize(meshesToRender.size() + 10);

	meshesToRender.at(numActive).mesh = pMesh;
	meshesToRender.at(numActive).transform = pTransform;

	++numActive;
}

void Renderer::SubmitLight(Okay::CompPointLight* pLight, Okay::CompTransform* pTransform)
{
	if (numLights >= lights.size())
		ExpandPointLights();

	lights.at(numLights).lightData = *pLight;
	lights.at(numLights).pos = pTransform->position;

	++numLights;
}

void Renderer::NewFrame()
{
	numActive = 0;
	numLights = 0;
}

void Renderer::Shutdown()
{
	shaderModel->Shutdown();

	DX11_RELEASE(pViewProjectBuffer);
	DX11_RELEASE(pWorldBuffer);
	DX11_RELEASE(pMaterialBuffer);

	DX11_RELEASE(pInputLayout);
	DX11_RELEASE(pVertexShader);
	DX11_RELEASE(pHullShader);
	DX11_RELEASE(pDomainShader);

	DX11_RELEASE(pPointLightBuffer);
	DX11_RELEASE(pPointLightSRV);
	DX11_RELEASE(pLightInfoBuffer);

#if ANIMATION == 1
	DX11_RELEASE(aniVS);
	DX11_RELEASE(aniIL);
	DX11_RELEASE(aniBuffer);
	DX11_RELEASE(aniSRV);
#endif
}

void Renderer::Render()
{
	using namespace Okay;

	shaderModel->Bind();
	mainCamera->Update();

	DX11::UpdateBuffer(pViewProjectBuffer, &mainCamera->GetViewProjectMatrix(), sizeof(DirectX::XMFLOAT4X4));

	if (numLights)
	{
		DX11::UpdateBuffer(pPointLightBuffer, lights.data(), UINT(sizeof(GPUPointLight) * numLights));
		DX11::UpdateBuffer(pLightInfoBuffer, &numLights, 4);
	}

	pDevContext->VSSetShader(pVertexShader, nullptr, 0);
	pDevContext->IASetInputLayout(pInputLayout);

	for (size_t i = 0; i < numActive; i++)
	{
		CompMesh& cMesh = *meshesToRender.at(i).mesh;
		const CompTransform& transform = *meshesToRender.at(i).transform;
		auto material = cMesh.GetMaterial();
		auto mesh = cMesh.GetMesh();

		DX11::UpdateBuffer(pWorldBuffer, &transform.matrix, sizeof(DirectX::XMFLOAT4X4));
		DX11::UpdateBuffer(pMaterialBuffer, &material->GetGPUData(), sizeof(MaterialGPUData));

		mesh->Bind();
		material->BindTextures();

		mesh->Draw();
	}


#if ANIMATION == 1
	// Temp
	CalculateAnimation(Okay::Engine::GetDT());

	pDevContext->IASetInputLayout(aniIL);
	pDevContext->IASetIndexBuffer(goblin->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDevContext->IASetVertexBuffers(0, Okay::SkeletalMesh::NumBuffers, goblin->vertexBuffer, Okay::SkeletalMesh::Stride, Okay::SkeletalMesh::Offset);

	pDevContext->VSSetShader(aniVS, nullptr, 0);
	pDevContext->VSSetShaderResources(0, 1, &aniSRV);

	pDevContext->DrawIndexed(goblin->numIndices, 0, 0);

	shaderModel->UnBind();
#endif
}

bool Renderer::ExpandPointLights()
{
	static const UINT Increase = 5;
	ID3D11Device* pDevice = DX11::Get().GetDevice();

	DX11_RELEASE(pPointLightBuffer);
	DX11_RELEASE(pPointLightSRV);

	lights.resize(numLights + Increase);

	VERIFY_HR_BOOL(DX11::CreateStructuredBuffer<GPUPointLight>(&pPointLightBuffer, lights.data(), (UINT)lights.size(), false));
	VERIFY_HR_BOOL(DX11::CreateStructuredSRV<GPUPointLight>(&pPointLightSRV, pPointLightBuffer, (UINT)lights.size()));

	pDevContext->PSSetShaderResources(3, 1, &pPointLightSRV);

	return true;
}

void Renderer::Bind()
{
	pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDevContext->IASetInputLayout(pInputLayout);

	pDevContext->VSSetShader(pVertexShader, nullptr, 0);
	pDevContext->VSSetConstantBuffers(0, 1, &pViewProjectBuffer);
	pDevContext->VSSetConstantBuffers(1, 1, &pWorldBuffer);
	pDevContext->PSSetConstantBuffers(3, 1, &pMaterialBuffer);

	pDevContext->PSSetShaderResources(3, 1, &pPointLightSRV);
	pDevContext->PSSetConstantBuffers(4, 1, &pLightInfoBuffer);



}

bool Renderer::CreateVS()
{
	std::string shaderData;

	D3D11_INPUT_ELEMENT_DESC desc[3] = {
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV",			0, DXGI_FORMAT_R32G32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	VERIFY(Okay::ReadShader("MeshVS.cso", shaderData));
	VERIFY_HR_BOOL(DX11::Get().GetDevice()->CreateInputLayout(desc, 3, shaderData.c_str(), shaderData.length(), &pInputLayout));
	VERIFY_HR_BOOL(DX11::Get().GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &pVertexShader));

#if ANIMATION == 1
	D3D11_INPUT_ELEMENT_DESC aniDesc[5] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"JOINTIDX", 0, DXGI_FORMAT_R32G32B32A32_UINT,  1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WEIGHTS",	 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV",		 0, DXGI_FORMAT_R32G32_FLOAT,		2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,	2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	VERIFY(Okay::ReadShader("SkeletalMeshVS.cso", shaderData));
	VERIFY_HR_BOOL(DX11::Get().GetDevice()->CreateInputLayout(aniDesc, 5, shaderData.c_str(), shaderData.length(), &aniIL));
	VERIFY_HR_BOOL(DX11::Get().GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &aniVS));
#endif

	return true;
}

bool Renderer::CreateHS()
{
	return false; // Disabled
}

bool Renderer::CreateDS()
{
	return false; // Disabled
}


#if ANIMATION == 1

int Renderer::FindJointIndex(std::vector<Joint>& joints, std::string_view name)
{
	for (int i = 0; i < (int)joints.size(); i++)
	{
		if (joints[i].name == name)
			return i;
	}

	return -1;
}

aiNode* Renderer::GetParentNode(std::vector<Joint>& joints, aiNode* child)
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

void Renderer::SetParents(std::vector<Joint>& joints, aiNode* node)
{
	int currentJointIdx = FindJointIndex(joints, node->mName.C_Str());
	if (currentJointIdx != -1)
	{
		aiNode* pParent = GetParentNode(joints, node);
		if (pParent)
			joints[currentJointIdx].parentIdx = FindJointIndex(joints, pParent->mName.C_Str());
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
		SetParents(joints, node->mChildren[i]);
}

void Renderer::CreateSkeletal()
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile("..\\Content\\Meshes\\ani\\gobwalk3.fbx",
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices);

	if (!pScene)
		return;

	auto ani = pScene->mAnimations[0];
	auto mesh = pScene->mMeshes[0];
	Okay::SkeletalMesh::VertexData data;

	// Mesh
	{
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
		UINT highest = 0;

		data.indices.resize(NumIndices);
		for (UINT i = 0; i < mesh->mNumFaces; i++)
		{
			data.indices[counter++] = mesh->mFaces[i].mIndices[0];
			data.indices[counter++] = mesh->mFaces[i].mIndices[1];
			data.indices[counter++] = mesh->mFaces[i].mIndices[2];


			if (mesh->mFaces[i].mIndices[0] > highest) highest = mesh->mFaces[i].mIndices[0];
			if (mesh->mFaces[i].mIndices[1] > highest) highest = mesh->mFaces[i].mIndices[1];
			if (mesh->mFaces[i].mIndices[2] > highest) highest = mesh->mFaces[i].mIndices[2];

		}

		int q = 0;
	}

	tickLengthS = 1.f / (float)ani->mTicksPerSecond;
	aniDurationS = (float)ani->mDuration / (float)ani->mTicksPerSecond;
	aniTime = 0.f;

	aniMatrices.resize(mesh->mNumBones);
	joints.resize(mesh->mNumBones);

	for (UINT i = 0; i < mesh->mNumBones; i++)
	{
		joints[i].name = mesh->mBones[i]->mName.C_Str();
		memcpy(&joints[i].invBindPose, &mesh->mBones[i]->mOffsetMatrix, sizeof(DirectX::XMFLOAT4X4));
		joints[i].invBindPose = DirectX::XMMatrixTranspose(joints[i].invBindPose);

		aiNodeAnim* channel = FindAnimNode(ani->mChannels, ani->mNumChannels, std::string_view(joints[i].name));
		if (!channel)
		{
			printf("%s: channel was NULL\n", joints[i].name.c_str());

			if (!FixJoint(joints[i], pScene->mRootNode))
				printf("%s: failed fixing", joints[i].name.c_str());

			continue;
		}

		// ASSUMING ALL STAMPS SAME LENGTH & TIME MATCHING
		if (channel->mNumPositionKeys != channel->mNumRotationKeys && 
			channel->mNumPositionKeys != channel->mNumScalingKeys &&
			channel->mNumRotationKeys != channel->mNumScalingKeys)
		{
			printf("%s: keys not matching\n", joints[i].name.c_str());
			if (!FixJoint(joints[i], pScene->mRootNode))
				printf("%s: failed fixing", joints[i].name.c_str());

			continue;
		}

		joints[i].stamps.resize(channel->mNumPositionKeys);

		for (UINT k = 0; k < channel->mNumPositionKeys; k++)
		{
			joints[i].stamps[k].time = (float)channel->mPositionKeys[k].mTime;

			joints[i].stamps[k].pos = channel->mPositionKeys[k].mValue;
			joints[i].stamps[k].rot = channel->mRotationKeys[k].mValue;
			joints[i].stamps[k].scale = channel->mScalingKeys[k].mValue;
		}
	}

	SetParents(joints, pScene->mRootNode);

	for (UINT i = 0; i < mesh->mNumBones; i++)
	{
		Joint& joint = joints[i];
		int q = 0;
		if (joint.stamps.size() < highestNumKeys || !jointHasTra[i])
		{
			joint.stamps.resize(highestNumKeys);
			const std::string nodeName(std::string(joint.name) + "_$AssimpFbx$_Translation");
			
			if (nodesMap.find(nodeName) != nodesMap.end())
			{
				printf("%s	|	", nodeName.c_str());
				printf("X: %f, Y: %f, Z: %f\n", nodesMap[nodeName]->mTransformation.a4,
					nodesMap[nodeName]->mTransformation.b4, nodesMap[nodeName]->mTransformation.c4);

				auto& n = nodesMap[nodeName];
				for (TimeStamp& stamp : joint.stamps)
				{
					stamp.pos.x = nodesMap[nodeName]->mTransformation.a4;
					stamp.pos.y = nodesMap[nodeName]->mTransformation.b4;
					stamp.pos.z = nodesMap[nodeName]->mTransformation.c4;
				}
			}
			else
			{
				printf("Else | %s\n", nodeName.c_str());


				for (TimeStamp& stamp : joint.stamps)
					stamp.pos = pos;

			}
		}
	}

	data.weights.resize(data.indices.size());
	for (UINT i = 0; i < mesh->mNumBones; i++)
	{
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

	for (Okay::SkinnedVertex& weight : data.weights)
	{
		float sum = weight.weight[0] + weight.weight[1] + weight.weight[2] + weight.weight[3];
		weight.weight[0] /= sum;
		weight.weight[1] /= sum;
		weight.weight[2] /= sum;
		weight.weight[3] /= sum;
	}


	goblin = std::make_unique<Okay::SkeletalMesh>(data);
}

void Renderer::CalculateAnimation(float dt)
{
	static size_t currentStamp = 0;
	static float tickTime = 0.f;

	tickTime += dt;
	aniTime += dt;

	if (tickTime > tickLengthS)
	{
		tickTime = 0.f;
		currentStamp++;
	}
	if (aniTime > aniDurationS || currentStamp >= joints[0].stamps.size())
	{
		aniTime = 0.f;
		currentStamp = 0;
	}

	//printf("Stamp: %zd\nTime: %f\n", currentStamp, aniTime);

	using namespace DirectX;


	TimeStamp& rootStamp = joints[0].stamps[currentStamp];

	joints[0].localT =
		XMMatrixScaling(rootStamp.scale.x, rootStamp.scale.y, rootStamp.scale.z) *
		XMMatrixRotationQuaternion(XMVectorSet(rootStamp.rot.x, rootStamp.rot.y, rootStamp.rot.z, rootStamp.rot.w)) *
		XMMatrixTranslation(rootStamp.pos.x, rootStamp.pos.y, rootStamp.pos.z);
	
	joints[0].modelT = joints[0].localT;
	joints[0].finalT = joints[0].invBindPose * joints[0].modelT;

	for (size_t i = 1; i < joints.size(); i++)
	{
		TimeStamp& stamp = joints[i].stamps.size() > 1 ? joints[i].stamps[currentStamp] : joints[i].stamps[0];

		joints[i].localT =
			XMMatrixScaling(stamp.scale.x, stamp.scale.y, stamp.scale.z) *
			XMMatrixRotationQuaternion(XMVectorSet(stamp.rot.x, stamp.rot.y, stamp.rot.z, stamp.rot.w)) *
			XMMatrixTranslation(stamp.pos.x, stamp.pos.y, stamp.pos.z);
		
		joints[i].modelT = joints[i].localT * joints[joints[i].parentIdx].modelT;
		joints[i].finalT = joints[i].invBindPose * joints[i].modelT;

	}

	for (size_t i = 0; i < joints.size(); i++)
		XMStoreFloat4x4(&aniMatrices[i], XMMatrixTranspose(joints[i].finalT));

	DX11::UpdateBuffer(aniBuffer, aniMatrices.data(), UINT(sizeof(XMFLOAT4X4) * aniMatrices.size()));

}

#endif