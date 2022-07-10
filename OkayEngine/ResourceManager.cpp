#include "ResourceManager.h"

Assets::Assets()
{

}

Assets::~Assets()
{

}

void Assets::AddMesh(const std::string& file)
{
}

std::shared_ptr<Okay::Mesh> Assets::GetMesh(const std::string& fileName)
{
	auto& ptr = meshes[fileName];

	if (!ptr.get())
		ptr = std::make_shared<Okay::Mesh>();

	return ptr;
}
