#pragma once
#include <vector>

#include "ShaderModel.h"
#include "Mesh.h"
#include "Camera.h"

class Renderer
{
public:
	Renderer();
	static Renderer& Get()
	{
		static Renderer renderer;
		return renderer;
	}

public:
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(const Renderer&) = delete;


	void Shutdown();
	void Render();


private:
	std::unique_ptr<Okay::ShaderModel> shaderModel;
	std::unique_ptr<Okay::Camera> mainCamera;

	//std::vector<Okay::Mesh*> meshesToRender;
	Okay::Mesh mesh;



private: // DX11 Specific
	ID3D11DeviceContext* pDevContext;

	ID3D11Buffer* pViewProjectBuffer;

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

#include <iostream>
using namespace std;

class Foo; // Forward declaration of class Foo in order for example to compile

class Bar {
private:
	int a = 0;
public:
	void show(Bar& x, Foo& y);




	// declaration of global friend
	friend void show(Bar& x, Foo& y); 
};

class Foo {
private:
	int b = 6;
public:
	friend void Bar::show(Bar& x, Foo& y); // declaration of friend from other class 



	// declaration of global friend
	friend void show(Bar& x, Foo& y); 
};

// Definition of a member function of Bar; this member is a friend of Foo
void Bar::show(Bar& x, Foo& y) {

	cout << "Show via function member of Bar" << endl;
	cout << "Bar::a = " << x.a << endl;
	cout << "Foo::b = " << y.b << endl;
}

// Friend for Bar and Foo, definition of global function
//void show(Bar& x, Foo& y) {
//	cout << "Show via global function" << endl;
//	cout << "Bar::a = " << x.a << endl;
//	cout << "Foo::b = " << y.b << endl;
//}
