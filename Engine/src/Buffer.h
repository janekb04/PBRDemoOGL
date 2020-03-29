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
	static void copy_sub_data(const Buffer& source, const Buffer& target, size_t read_offset, size_t write_offset, size_t size)
	{
		glCopyNamedBufferSubData(source.handle, target.handle, read_offset, write_offset, size);
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
		return glMapNamedBufferRange(handle, offset, length, access);
	}

	void sub_data(size_t offset, size_t size, const void* data)
	{
		glNamedBufferSubData(handle, offset, size, data);
	}

	void get_sub_data(size_t offset, size_t size, void* data)
	{
		glGetNamedBufferSubData(handle, offset, size, data);
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
	
	bool is_null() const
	{
		return handle == 0;
	}

	~Buffer()
	{
		glDeleteBuffers(1, &handle);
	}
};