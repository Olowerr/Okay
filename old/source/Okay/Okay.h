#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "OkayVertices.h"
#include "Defines.h"

namespace Okay
{
	static const DirectX::XMVECTOR RIGHT = DirectX::XMVectorSet(1.f, 0.f, 0.f, 0.f);
	static const DirectX::XMVECTOR UP = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
	static const DirectX::XMVECTOR FORWARD = DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f);

	static const Float3 WHITE	= Float3(1.f, 1.f, 1.f);
	static const Float3 RED		= Float3(1.f, 0.f, 0.f);
	static const Float3 GREEN	= Float3(0.f, 1.f, 0.f);
	static const Float3 BLUE	= Float3(0.f, 0.f, 1.f);

	struct String
	{
		static const UINT Size = 64;

		String() = default;
		String(const char* string) {
			memcpy(c_str, string, GetMinLength(string));
		}
		String(const std::string& string) {
			memcpy(c_str, string.c_str(), GetMinLength(string.c_str()));
		}

		bool operator==(const String& other) const
		{
			return !memcmp(c_str, other.c_str, Size);
		}
		bool operator==(const std::string& string) const
		{
			const int StringLength = (int)string.length();
			if (StringLength > Size)
				return false;

			const int NullTerminatorPos = GetNullTerminatorPos();
			if (NullTerminatorPos != StringLength)
				return false;

			return !memcmp(c_str, string.c_str(), NullTerminatorPos);
		}
		bool operator==(const char* string) const
		{
			return c_str == std::string(string);
		}

		operator const char* () const { return c_str; }
		operator char* () { return c_str; }

		char c_str[Size]{};

	private:
		static UINT GetMinLength(const char* string)
		{
			return (UINT)strlen(string) < Size ? (UINT)strlen(string) : Size;
		}

		int GetNullTerminatorPos() const
		{
			for (int i = 0; i < Size; i++)
			{
				if (c_str[i] == '\0')
					return i;
			}
			return -1;
		}
	};
	
	struct VertexData
	{
		std::vector<Okay::Float3> position;
		std::vector<Okay::UVNormal> uvNormal;
		std::vector<UINT> indices;
	};

	bool ReadShader(const std::string& shaderPath, std::string& output);
}