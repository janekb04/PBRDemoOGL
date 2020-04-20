#pragma once

#include "Vendor.h"

class Renderbuffer
{
private:
	GLuint handle;

	static GLuint create_handle()
	{
		GLuint handle;
		glCreateRenderbuffers(1, &handle);
		return handle;
	}
public:
	Renderbuffer() :
		handle{ create_handle() }
	{
	}

	Renderbuffer(const Renderbuffer&) = delete;

	Renderbuffer(Renderbuffer&& other) noexcept :
		handle(other.handle)
	{
		other.handle = 0;
	}

	operator GLuint() const
	{
		return handle;
	}

	void storage(GLenum internal_format, unsigned width, unsigned height)
	{
		glNamedRenderbufferStorage(handle, internal_format, width, height);
	}
};