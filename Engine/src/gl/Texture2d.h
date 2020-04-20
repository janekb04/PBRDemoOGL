#pragma once

#include "../Vendor.h"
#include "GLObject.h"
#include "TextureHandle.h"

class Texture2d : public GLObject<Texture2d>
{
private:
	friend class GLObject<Texture2d>;
	static void create(GLsizei count, GLuint* handles)
	{
		glCreateTextures(GL_TEXTURE_2D, count, handles);
	}
	static void destroy(GLsizei count, GLuint* handles)
	{
		glDeleteTextures(count, handles);
	}
private:
	Texture2d(GLuint handle) :
		GLObject<Texture2d>{handle}
	{
	}
public:
	static Texture2d view(GLenum target, const Texture2d& original, GLenum internal_format, unsigned first_mip_level, unsigned mip_level_count, unsigned first_layer, unsigned layer_count)
	{
		GLuint handle;
		glGenTextures(1, &handle);
		glTextureView(handle, target, original, internal_format, first_mip_level, mip_level_count, first_layer, layer_count);
		return Texture2d(handle);
	}
public:
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
};