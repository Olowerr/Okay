#pragma once

#include <string>
#include <fstream>

#include "Defines.h"

namespace Okay
{
	bool ReadShader(const std::string& shaderPath, std::string& output);

}