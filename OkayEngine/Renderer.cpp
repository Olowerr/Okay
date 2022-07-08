#include "Renderer.h"

Renderer::Renderer()
{
	shaderModel = new Okay::ShaderModel;
}

Renderer::~Renderer()
{
	Shutdown();
}

void Renderer::Shutdown()
{
	shaderModel->Shutdown();
	OkayDelete(shaderModel);
}