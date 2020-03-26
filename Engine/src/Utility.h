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

double rand01()
{
	return ((double)rand() / (RAND_MAX));
}

template <typename T> int sgn(const T& val) {
	return (T(0) < val) - (val < T(0));
}

template <class Adapter>
typename Adapter::container_type& get_container(Adapter& a)
{
	struct hack : Adapter {
		static typename Adapter::container_type& get(Adapter& a) {
			return a.*&hack::c;
		}
	};
	return hack::get(a);
}

template<class T, class... Args>
constexpr T* construct_at(T* p, Args&&... args) // replacement for C++20 std::construct_at
{
	return ::new (const_cast<void*>(static_cast<const volatile void*>(p)))
		T(std::forward<Args>(args)...);
}