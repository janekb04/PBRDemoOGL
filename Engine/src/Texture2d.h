#pragma once

#include "Vendor.h"
#include "Image2d.h"
#include "TextureHandle.h"

class Texture2d
{
private:
	GLuint handle;
private:
	static GLuint create_texture()
	{
		GLuint handle;
		glCreateTextures(GL_TEXTURE_2D, 1, &handle);
		return handle;
	}
	Texture2d(GLuint handle) :
		handle(handle)
	{
	}
public:
	Texture2d() :
		handle(create_texture())
	{
	}

	Texture2d(const Texture2d&) = delete;

	Texture2d(Texture2d&& other) noexcept:
		handle(other.handle)
	{
		other.handle = 0;
	}

	operator GLuint() const
	{
		return handle;
	}

	void storage(unsigned mip_levels, GLenum internal_format, unsigned width, unsigned height) const
	{
		glTextureStorage2D(handle, mip_levels, internal_format, width, height);
	}

	void sub_image(unsigned mip_level, unsigned xoffset, unsigned yoffset, unsigned width, unsigned height, GLenum format, GLenum type, const void* pixels)
	{
		glTextureSubImage2D(handle, mip_level, xoffset, yoffset, width, height, format, type, pixels);
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
		glBindTexture(GL_TEXTURE_2D, handle);
	}

	~Texture2d()
	{
		if(handle)
			glDeleteTextures(1, &handle);
	}
};

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