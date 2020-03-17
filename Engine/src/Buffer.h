#pragma once
#include "Vendor.h"

class Buffer
{
private:
	GLuint handle;

	static GLuint create_handle()
	{
		GLuint handle;
		glCreateBuffers(1, &handle);
		return handle;
	}
public:
	Buffer() :
		handle{ create_handle() }
	{
	}

	void data(size_t size, const void* data, GLenum usage) const
	{
		glNamedBufferData(handle, size, data, usage);
	}

	GLuint get_handle() const
	{
		return handle;
	}

	void bind(GLenum target) const
	{
		glBindBuffer(target, handle);
	}

	~Buffer()
	{
		glDeleteBuffers(1, &handle);
	}
};