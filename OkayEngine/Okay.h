#pragma once

#include <string>
#include <fstream>

#include "Defines.h"

namespace Okay
{
	struct String
	{
		static const UINT Size = 64;

		String() = default;
		String(const char* string) {
			const UINT len = (UINT)strlen(string) < Size ? (UINT)strlen(string) : Size;
			memcpy(str, string, len);
		}
		String(const std::string& string) {
			const UINT len = (UINT)string.length() < Size ? (UINT)string.length() : Size;
			memcpy(str, string.c_str(), len);
		}

		char str[Size]{};
	};
	
	bool ReadShader(const std::string& shaderPath, std::string& output);


}