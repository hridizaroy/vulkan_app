#pragma once

#include "config.h"

namespace vkUtil
{
	std::vector<char> readFile(std::string filename, bool debug)
	{
		// start the stream at end of file, in order to get file size
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (debug && !file.is_open())
		{
			std::cout << "Failed to load \"" << filename << "\"" << std::endl;
		}

		// Get number of bytes
		size_t filesize{ static_cast<size_t>(file.tellg()) };

		std::vector<char> buffer(filesize);

		// Go to start of file
		file.seekg(0);
		file.read(buffer.data(), filesize);

		file.close();

		return buffer;
	}
}
