#include "Okay.h"

bool Okay::ReadShader(const std::string& shaderPath, std::string& output)
{
    std::ifstream reader(ShaderPath + shaderPath, std::ios::binary);
    VERIFY(reader);

    reader.seekg(0, std::ios::end);
    output.reserve(reader.tellg());
    reader.seekg(0, std::ios::beg);

    output.assign(std::istream_iterator<char>(reader), std::istream_iterator<char>());

    return true;
}
