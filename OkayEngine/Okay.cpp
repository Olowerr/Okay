#include "Okay.h"

bool Okay::ReadShader(const std::string& shaderPath, std::string& output)
{
    std::ifstream reader(ShaderPath + shaderPath, std::ios::binary);
    VERIFY(reader);

    reader.seekg(0, std::ios::end);
    output.reserve((size_t)reader.tellg());
    reader.seekg(0, std::ios::beg);

    output.assign(std::istreambuf_iterator<char>(reader), std::istreambuf_iterator<char>());

    return true;
}
