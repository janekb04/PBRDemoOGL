#pragma once

#include "Vendor.h"
#include "Image2d.h"
#include "Image2dArray.h"
#include "gl/Texture2d.h"
#include "gl/Texture2dArray.h"

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

template <typename T>
T RGBtoSRGB(const T& color)
{
	return pow(color, T{ 1 / 2.2 });
}

template <typename T>
T SRGBtoRGB(const T& color)
{
	return pow(color, T{ 2.2 });
}

Texture2d texture2d_from_image(const Image2d& image, unsigned mip_levels = 1)
{
	Texture2d texture;

	texture.storage(mip_levels, image.internal_format(), image.width(), image.height());

	GLenum formats[]
	{
		GL_RED,
		GL_RG,
		GL_RGB,
		GL_RGBA,
	};
	texture.sub_image(0, 0, 0, image.width(), image.height(), formats[image.channels() - 1], GL_UNSIGNED_BYTE, image.data());

	if (mip_levels > 1)
		texture.generate_mipmap();

	return texture;
}

GLObject<Texture2d> x()
{
	GLObject<Texture2d> o;
	return o;
}

Texture2dArray texture2d_array_from_image_array(const Image2dArray& images, int mipmap_count = 1)
{
	Texture2dArray texture_array;

	texture_array.storage(mipmap_count, images.internal_format(), images.width(), images.height(), images.depth());
	GLenum formats[]
	{
		GL_RED,
		GL_RG,
		GL_RGB,
		GL_RGBA,
	};
	texture_array.sub_image(0, 0, 0, 0, images.width(), images.height(), images.depth(), formats[images.channels() - 1], GL_UNSIGNED_BYTE, images.data());
	texture_array.generate_mipmap();

	return texture_array;
}