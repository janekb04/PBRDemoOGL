#pragma once

#include "../Vendor.h"
#include "GLObject.h"
#include "TextureHandle.h"

class Texture2dArray : public GLObject<Texture2dArray>
{
private:
	friend class GLObject<Texture2dArray>;
	static void create(GLsizei count, GLuint* handles)
	{
		glCreateTextures(GL_TEXTURE_2D_ARRAY, count, handles);
	}
	static void destroy(GLsizei count, GLuint* handles)
	{
		glDeleteTextures(count, handles);
	}
public:
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
};