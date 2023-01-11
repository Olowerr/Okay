#pragma once
#include <string>

namespace Okay
{
	struct String
	{
		static const uint32_t LENGTH = 64;

		String() = default;
		String(const char* str)
		{
			copy(str);
		}
		String(const std::string& str)
		{
			copy(str.c_str());
		}
		String(std::string_view str)
		{
			copy(str);
		}

		String& operator=(const char* str)
		{
			copy(str);
			return *this;
		}
		String& operator=(const std::string& str)
		{
			copy(str.c_str());
			return *this;
		}
		String& operator=(const std::string_view& str)
		{
			copy(str);
			return *this;
		}

		bool operator==(const char* str)
		{
			return strcmp(cStr, str) == 0;
		}
		bool operator==(const std::string& str)
		{
			return strcmp(cStr, str.c_str()) == 0;
		}
		bool operator==(const std::string_view& str)
		{
			return memcmp(cStr, str.data(), str.length()) == 0;
		}

		char& operator[](int index) { return cStr[index]; }

		operator char* () { return cStr; }
		operator const char* () { return cStr; } const
		
		void copy(const char* str)
		{
			size_t length = strlen(str);
			if (length > LENGTH)
				length = LENGTH;

			memcpy(cStr, str, length);
		}
		void copy(const std::string_view& str)
		{
			size_t length = str.length();
			if (length > LENGTH)
				length = LENGTH;

			memcpy(cStr, str.data(), length);
		}

		void clear()
		{
			memset(cStr, '\0', LENGTH);
		}

		size_t length() const
		{
			return strlen(cStr);
		}

		char cStr[LENGTH]{};
	};
}