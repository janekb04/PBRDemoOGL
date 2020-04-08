#pragma once

#include "Vendor.h"
#include "Buffer.h"

template <typename T>
class BufferArray
{
private:
	Buffer m_buffer;
	size_t m_size, m_capacity;
public:
	using handle = size_t;
public:
	BufferArray(size_t max_size) :
		m_size(0),
		m_capacity(max_size)
	{
		m_buffer.storage(sizeof(T) * max_size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	size_t size() const noexcept
	{
		return m_size;
	}

	size_t capacity() const noexcept
	{
		return m_capacity;
	}

	handle add_multiple(const T* first, size_t count)
	{
		assert(m_size + count <= m_capacity);

		m_buffer.sub_data(sizeof(T) * m_size, sizeof(T) * count, first);
		auto h = m_size;
		m_size += count;
		return h;
	}

	handle add(const T& elem)
	{
		return add_multiple(&elem, 1);
	}

	void get_multiple(handle h_first, size_t count, T* dest) const
	{
		assert(h_first + count < m_size);
		
		m_buffer.get_sub_data(sizeof(T) * h_first, sizeof(T) * count, dest);
	}

	T get(handle h) const
	{
		T temp;
		get_multiple(h, 1, &temp);
		return temp;
	}

	const Buffer& buffer() const
	{
		return m_buffer;
	}

	void set_multiple(handle h_first, size_t count, const T* data) const
	{
		assert(h_first + count < m_size);

		m_buffer.sub_data(sizeof(T) * h_first, sizeof(T) * count, data);
	}

	void set(handle h, const T& data) const
	{
		set_multiple(h, 1, &data);
	}
};