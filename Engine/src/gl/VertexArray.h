#pragma once

#include "../Vendor.h"
#include "Buffer.h"

class VertexAttribute
{
private:
	GLuint m_index;
public:
	explicit VertexAttribute(unsigned int location) :
		m_index{ location }
	{
	}

	unsigned int index() const
	{
		return m_index;
	}
};

class VertexArray
{
private:
	GLuint handle;

	static GLuint create_handle()
	{
		GLuint handle;
		glCreateVertexArrays(1, &handle);
		return handle;
	}
public:
	VertexArray() :
		handle{ create_handle() }
	{
	}

	void bind() const
	{
		glBindVertexArray(handle);
	}

	void vertex_buffer(unsigned int binding_index, const Buffer& buffer, ptrdiff_t offset, size_t stride)
	{
		glVertexArrayVertexBuffer(handle, binding_index, buffer.get_handle(), offset, stride);
	}

	void element_buffer(const Buffer& buffer)
	{
		glVertexArrayElementBuffer(handle, buffer.get_handle());
	}

	void enable_attrib(VertexAttribute attribute)
	{
		glEnableVertexArrayAttrib(handle, attribute.index());
	}

	void attrib_binding(VertexAttribute attribute, unsigned int binding_index)
	{
		glVertexArrayAttribBinding(handle, attribute.index(), binding_index);
	}

	void attrib_format(VertexAttribute attribute, int size, GLenum type, bool normalized, unsigned int relative_offset)
	{
		glVertexArrayAttribFormat(handle, attribute.index(), size, type, normalized, relative_offset);
	}

	GLuint get_handle() const
	{
		return handle;
	}

	~VertexArray()
	{
		glDeleteVertexArrays(1, &handle);
	}
};