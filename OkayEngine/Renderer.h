#pragma once
#include <vector>

#include "ShaderModel.h"
#include "Mesh.h"
#include "Camera.h"

class Renderer
{
public:
	static Renderer& Get()
	{
		static Renderer renderer;
		return renderer;
	}
	void Shutdown();

	void Render();

private:
	std::unique_ptr<Okay::ShaderModel> shaderModel;
	std::unique_ptr<Okay::Camera> mainCamera;

	//std::vector<Okay::Mesh*> meshesToRender;
	Okay::Mesh triangle;



private: // DX11 Specific
	ID3D11DeviceContext* pDevContext;

	ID3D11Buffer* pViewProjectBuffer;

	ID3D11InputLayout* pInputLayout;
	ID3D11VertexShader* pVertexShader;
	ID3D11HullShader* pHullShader;     // Disabled
	ID3D11DomainShader* pDomainShader; // Disabled

	void Bind();

private:
	Renderer();
public:
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(const Renderer&) = delete;


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