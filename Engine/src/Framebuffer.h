#pragma once

#include "Vendor.h"
#include "Texture2d.h"

class Framebuffer
{
private:
	GLuint handle;

	static GLuint create_handle()
	{
		GLuint handle;
		glCreateFramebuffers(1, &handle);
		return handle;
	}
public:
	Framebuffer() :
		handle{ create_handle() }
	{
	}

	Framebuffer(const Framebuffer&) = delete;

	Framebuffer(Framebuffer&& other) :
		handle(other.handle)
	{
		other.handle = 0;
	}

	void bind(GLenum target) const
	{
		glBindFramebuffer(target, handle);
	}

	void draw_buffers(size_t count, GLenum* buffers)
	{
		glNamedFramebufferDrawBuffers(handle, count, buffers);
	}

	void texture(GLenum attachment, const Texture2d& texture, unsigned mip_level)
	{
		glNamedFramebufferTexture(handle, attachment, texture, mip_level);
	}

	GLenum check_status(GLenum target)
	{
		glCheckNamedFramebufferStatus(handle, target);
	}
};