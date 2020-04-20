#pragma once

#include "../Vendor.h"
#include "GLObject.h"

class Buffer : public GLObject<Buffer>
{
private:
	friend class GLObject<Buffer>;
	static void create(GLsizei count, GLuint* handles)
	{
		glCreateBuffers(count, handles);
	}
	static void destroy(GLsizei count, GLuint* handles)
	{
		glDeleteBuffers(count, handles);
	}
public:
	static void copy_sub_data(const Buffer& source, const Buffer& target, size_t read_offset, size_t write_offset, size_t size)
	{
		glCopyNamedBufferSubData(source.handle, target.handle, read_offset, write_offset, size);
	}
public:
	void data(size_t size, const void* data, GLenum usage)
	{
		glNamedBufferData(handle, size, data, usage);
	}

	void storage(size_t size, const void* data, GLbitfield flags)
	{
		glNamedBufferStorage(handle, size, data, flags);
	}

	void* map_range(size_t offset, size_t length, GLbitfield access) const
	{
		return glMapNamedBufferRange(handle, offset, length, access);
	}

	void sub_data(size_t offset, size_t size, const void* data) const
	{
		glNamedBufferSubData(handle, offset, size, data);
	}

	void get_sub_data(size_t offset, size_t size, void* data) const
	{
		glGetNamedBufferSubData(handle, offset, size, data);
	}

	void unmap()
	{
		if (!glUnmapNamedBuffer(handle))
			throw std::runtime_error("data store contents have become corrupt during the time the data store was mapped");
	}

	void bind(GLenum target) const
	{
		glBindBuffer(target, handle);
	}

	void bind_base(GLenum target, unsigned index) const
	{
		glBindBufferBase(target, index, handle);
	}
};