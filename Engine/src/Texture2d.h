#pragma once

#include "Vendor.h"
#include "Image2d.h"

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

	static Texture2d from_image(const Image2d& image)
	{
		GLuint handle = create_texture();

		glTextureStorage2D(handle, 1, image.internal_format(), image.width(), image.height());

		GLenum formats[]
		{
			GL_RED,
			GL_RG,
			GL_RGB,
			GL_RGBA,
		};
		glTextureSubImage2D(handle, 0, 0, 0, image.width(), image.height(), formats[image.channels() - 1], GL_UNSIGNED_BYTE, image.data());

		return Texture2d(handle);
	}

	void bind() const
	{
		glBindTexture(GL_TEXTURE_2D, handle);
	}

	~Texture2d()
	{
		glDeleteTextures(1, &handle);
	}
};