#pragma once

#include "Vendor.h"
#include "Image2dArray.h"

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
	Texture2dArray(const Texture2dArray&) = delete;

	static Texture2dArray from_image_array(const Image2dArray& images, int mipmap_count = 1)
	{
		GLuint handle = create_texture();
		
		glTextureStorage3D(handle, mipmap_count, images.internal_format(), images.width(), images.height(), images.depth());
		GLenum formats[]
		{
			GL_RED,
			GL_RG,
			GL_RGB,
			GL_RGBA,
		};
		glTextureSubImage3D(handle, 0, 0, 0, 0, images.width(), images.height(), images.depth(), formats[images.channels() - 1], GL_UNSIGNED_BYTE, images.data());

		glGenerateTextureMipmap(handle);

		return Texture2dArray(handle);
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