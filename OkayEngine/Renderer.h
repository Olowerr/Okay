#pragma once
#include <vector>

#include "ShaderModel.h"
#include "Mesh.h"

class Renderer
{
public:
	static Renderer& Get()
	{
		static Renderer renderer;
		return renderer;
	}
	void Shutdown();



private:
	Okay::ShaderModel* shaderModel;

	std::vector<Okay::Mesh*> meshesToRender;

	


private:
	Renderer();
public:
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(const Renderer&) = delete;
};