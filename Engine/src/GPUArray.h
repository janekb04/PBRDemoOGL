#pragma once

#include "Vendor.h"
#include "Utility.h"
#include "Buffer.h"

template <typename T>
class GPUArray
{
public:
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
private:
	size_type m_size;
	size_type m_capacity;
	Buffer m_buffer;
private:
	void allocate_buffer(size_t size) const
	{
		size_t size_in_basic_machine_units = size * sizeof(T);
		m_buffer.storage(size_in_basic_machine_units * sizeof(T), nullptr, 0);
	}
public:
	GPUArray(size_type count) : // only allocates
		m_size{ 0 },
		m_capacity{ count },
		m_buffer{}
	{
		allocate_buffer(m_capacity);
	}

	GPUArray(const GPUArray& other) :
		m_size{ other.m_size },
		m_capacity{ other.m_capacity },
		m_buffer{}
	{
		allocate_buffer(m_capacity);
		Buffer::copy_sub_data(other.m_buffer, m_buffer, 0, 0, m_size);
	}

	GPUArray(GPUArray&& other) noexcept :
		m_size{ other.m_size },
		m_capacity{ other.m_capacity },
		m_buffer{ std::move(other.m_buffer) }
	{
		other.m_size = 0;
		other.m_capacity = 0;
	}
	~GPUArray() = default;

	GPUArray& operator=(const GPUArray& other);
	GPUArray& operator=(GPUArray&& other) noexcept;
	GPUArray& operator=(std::initializer_list<T> init);

	void assign(size_type count, const_reference value);
	template<class InputIt>
	void assign(InputIt first, InputIt last);
	void assign(std::initializer_list<T> ilist);

	reference at(size_type pos);
	const_reference at(size_type pos) const;

	reference operator[](size_type pos)
	{
		return m_data_ptr[pos];
	}
	const_reference operator[](size_type pos) const
	{
		return m_data_ptr[pos];
	}

	reference front();
	const_reference front() const;

	reference back();
	const_reference back() const;

	T* data() noexcept
	{
		return m_data_ptr;
	}
	const T* data() const noexcept
	{
		return m_data_ptr;
	}

	iterator begin() noexcept
	{
		return m_data_ptr;
	}
	const_iterator begin() const noexcept
	{
		return m_data_ptr;
	}
	const_iterator cbegin() const noexcept
	{
		return m_data_ptr;
	}

	iterator end() noexcept
	{
		return m_data_ptr + m_size;
	}
	const_iterator end() const noexcept
	{
		return m_data_ptr + m_size;
	}
	const_iterator cend() const noexcept
	{
		return m_data_ptr + m_size;
	}

	reverse_iterator rbegin() noexcept;
	const_reverse_iterator rbegin() const noexcept;
	const_reverse_iterator crbegin() const noexcept;

	reverse_iterator rend() noexcept;
	const_reverse_iterator rend() const noexcept;
	const_reverse_iterator crend() const noexcept;

	bool empty() const noexcept
	{
		return m_size == 0;
	}

	size_type size() const noexcept
	{
		return m_size;
	}

	size_type max_size() const noexcept
	{
		return std::numeric_limits<size_t>::max();
	}

	size_type capacity() const noexcept
	{
		return m_capacity;
	}

	void clear() noexcept
	{
		std::destroy(begin(), end());
		m_size = 0;
	}

	iterator insert(const_iterator pos, const T& value);
	iterator insert(const_iterator pos, T&& value);
	iterator insert(const_iterator pos, size_type count, const T& value);
	template< class InputIt >
	void insert(iterator pos, InputIt first, InputIt last);
	template< class InputIt >
	iterator insert(const_iterator pos, InputIt first, InputIt last);
	iterator insert(const_iterator pos, std::initializer_list<T> ilist);

	template< class... Args >
	iterator emplace(const_iterator pos, Args&&... args);

	iterator erase(const_iterator pos);
	iterator erase(const_iterator first, const_iterator last);

	iterator push_back(const T& value)
	{
		assert(m_size < m_capacity);
		construct_at(m_data_ptr + m_size, value);
		++m_size;
		return std::prev(end());
	}
	iterator push_back(T&& value)
	{
		assert(m_size < m_capacity);
		construct_at(m_data_ptr + m_size, std::move(value));
		++m_size;
		return std::prev(end());
	}

	template< class... Args >
	reference emplace_back(Args&&... args)
	{
		assert(m_size < m_capacity);
		construct_at(m_data_ptr + m_size, std::forward<Args>(args)...);
		++m_size;
		return *std::prev(end());
	}

	void pop_back()
	{
		assert(m_size > 0);
		std::destroy_at(std::prev(end()));
		--m_size;
	}

	void resize(size_type count);
	void resize(size_type count, const value_type& value);

	void swap(GPUArray& other) noexcept;

	Buffer& get_buffer()
	{
		return m_buffer;
	}

	const Buffer& get_buffer() const
	{
		return m_buffer;
	}

	void unmap()
	{
		m_buffer.unmap();
		m_size = 0;
		m_capacity = 0;
		m_data_ptr = nullptr;
	}
};