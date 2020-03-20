#pragma once

#include "Vendor.h"

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
	static Texture2d from_file(const std::string& path)
	{
		int width, height, channels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		if (!data)
			throw std::runtime_error("failed to load texture");

		GLuint handle = create_texture();

		GLenum internal_formats[]
		{
			GL_R8,
			GL_RG8,
			GL_RGB8,
			GL_RGBA8,
		};
		glTextureStorage2D(handle, 1, internal_formats[channels - 1], width, height);
		GLenum formats[]
		{
			GL_RED,
			GL_RG,
			GL_RGB,
			GL_RGBA,
		};
		glTextureSubImage2D(handle, 0, 0, 0, width, height, formats[channels - 1], GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);

		return Texture2d(handle);
	}

	~Texture2d()
	{
		glDeleteTextures(1, &handle);
	}
};