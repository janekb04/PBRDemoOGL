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
	Texture2d(const Texture2d&) = delete;
	Texture2d(Texture2d&& other) :
		handle(other.handle)
	{
		other.handle = 0;
	}

	Texture2d(const Image2d& image, int mip_levels = 1) :
		handle(create_texture())
	{
		glTextureStorage2D(handle, mip_levels, image.internal_format(), image.width(), image.height());

		GLenum formats[]
		{
			GL_RED,
			GL_RG,
			GL_RGB,
			GL_RGBA,
		};
		glTextureSubImage2D(handle, 0, 0, 0, image.width(), image.height(), formats[image.channels() - 1], GL_UNSIGNED_BYTE, image.data());

		if (mip_levels > 1)
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