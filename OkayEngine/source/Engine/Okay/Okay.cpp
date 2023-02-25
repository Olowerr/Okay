#include "Okay.h"

#include <fstream>

namespace Okay
{
	bool readBinary(std::string_view binName, std::string& output)
	{
		std::ifstream reader(binName.data(), std::ios::binary);
		OKAY_VERIFY(reader);

		reader.seekg(0, std::ios::end);
		output.reserve((size_t)reader.tellg());
		reader.seekg(0, std::ios::beg);

		output.assign(std::istreambuf_iterator<char>(reader), std::istreambuf_iterator<char>());

		return true;
	}
}

