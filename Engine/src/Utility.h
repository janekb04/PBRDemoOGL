#pragma once

#include "Vendor.h"

std::string read_file(const char* filename)
{
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	if (file)
	{
		std::string contents;
		file.seekg(0, std::ios::end);
		contents.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(contents.data(), contents.size());
		file.close();
		return contents;
	}
	throw std::runtime_error("failed to open file");
}