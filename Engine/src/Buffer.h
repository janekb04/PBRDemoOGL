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

	Buffer(const Buffer&) = delete;

	Buffer(Buffer&& other) :
		handle(other.handle)
	{
		other.handle = 0;
	}

	void data(size_t size, const void* data, GLenum usage) const
	{
		glNamedBufferData(handle, size, data, usage);
	}

	void storage(size_t size, const void* data, GLbitfield flags) const
	{
		glNamedBufferStorage(handle, size, data, flags);
	}

	void* map_range(size_t offset, size_t length, GLbitfield access) const
	{
		glMapNamedBufferRange(handle, offset, length, access);
	}

	void unmap()
	{
		if (!glUnmapNamedBuffer(handle))
			throw std::runtime_error("data store contents have become corrupt during the time the data store was mapped");
	}

	GLuint get_handle() const
	{
		return handle;
	}

	void bind(GLenum target) const
	{
		glBindBuffer(target, handle);
	}

	void bind_base(GLenum target, unsigned index)
	{
		glBindBufferBase(target, index, handle);
	}

	~Buffer()
	{
		glDeleteBuffers(1, &handle);
	}
};