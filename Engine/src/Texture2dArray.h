#pragma once

#include "Vendor.h"
#include "Image2dArray.h"
#include "TextureHandle.h"

class Texture2dArray
{
private:
	GLuint handle;
private:
	static GLuint create_texture()
	{
		GLuint handle;
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &handle);
		return handle;
	}
	Texture2dArray(GLuint handle) :
		handle(handle)
	{
	}
public:
	Texture2dArray() :
		handle(create_texture())
	{
	}

	Texture2dArray(const Texture2dArray&) = delete;
	Texture2dArray(Texture2dArray&& other) noexcept :
		handle(other.handle)
	{
		other.handle = 0;
	}

	operator GLuint() const
	{
		return handle;
	}

	void storage(unsigned mip_levels, GLenum internal_format, unsigned width, unsigned height, unsigned depth) const
	{
		glTextureStorage3D(handle, mip_levels, internal_format, width, height, depth);
	}

	void sub_image(unsigned mip_level, unsigned xoffset, unsigned yoffset, unsigned zoffset, unsigned width, unsigned height, unsigned depth, GLenum format, GLenum type, const void* pixels)
	{
		glTextureSubImage3D(handle, mip_level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
	}

	void generate_mipmap()
	{
		glGenerateTextureMipmap(handle);
	}

	TextureHandle get_texture_handle() const
	{
		return TextureHandle(glGetTextureHandleARB(handle));
	}

	void bind() const
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, handle);
	}

	~Texture2dArray()
	{
		glDeleteTextures(1, &handle);
	}
};

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