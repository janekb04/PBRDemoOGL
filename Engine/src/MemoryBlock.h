#pragma once

#include <memory>
#include <algorithm>

template <typename T, typename Allocator = std::allocator<T>>
class memory_block
{
private:
	Allocator m_allocator;
	size_t m_size;
	T* m_data;
public:
	memory_block() :
		m_allocator{},
		m_size{ 0 },
		m_data{ nullptr }
	{
	}
	explicit memory_block(const Allocator& allocator) :
		m_allocator{ allocator },
		m_size{ 0 },
		m_data{ nullptr }
	{
	}

	memory_block(size_t count, const T& value, const Allocator& allocator = Allocator()) :
		m_allocator{ allocator },
		m_size{ count },
		m_data{ m_allocator.allocate(m_size) }
	{
		std::uninitialized_fill_n(m_data, m_size, value);
	}

	explicit memory_block(size_t count, const Allocator& allocator = Allocator()) :
		m_allocator{ allocator },
		m_size{ count },
		m_data{ m_allocator.allocate(m_size) }
	{
		std::uninitialized_default_construct_n(m_data, m_size);
	}
	
	template<class InputIt>
	memory_block(InputIt first, InputIt last, const Allocator& allocator = Allocator()) :
		m_allocator{allocator},
		m_size{ std::distance(first, last) },
		m_data{ m_allocator.allocate(m_size) }
	{
		std::uninitialized_copy(first, last, m_data);
	}
	
	memory_block(const memory_block& other) :
		memory_block(other, std::allocator_traits<Allocator>::select_on_container_copy_construction(other.get_allocator()))
	{
	}

	memory_block(const memory_block& other, const Allocator& allocator) :
		m_allocator{ allocator },
		m_size{ other.m_size },
		m_data{ m_allocator.allocate(m_size) }
	{
		std::uninitialized_copy_n(other.m_data, m_size, m_data);
	}
	
	memory_block(memory_block&& other) noexcept :
		m_allocator{ std::move(other.m_allocator) },
		m_size{ other.m_size },
		m_data{ other.m_data }
	{
		other.m_size = 0;
		other.m_data = nullptr;
	}
	
	memory_block(memory_block&& other, const Allocator& allocator) :
		m_allocator{ allocator },
		m_size{ other.m_size },
		m_data{ other.m_data }
	{
		other.m_size = 0;
		other.m_data = nullptr;
	}

	memory_block(std::initializer_list<T> init, const Allocator& allocator = Allocator()) :
		memory_block(init.begin(), init.end(), allocator)
	{
	}

	~memory_block()
	{
		if (m_data)
		{
			std::destroy_n(m_data, m_size);
			m_allocator.deallocate(m_data, m_size);
		}
	}

	memory_block& operator=(const memory_block& other);

	memory_block& operator=(memory_block&& other);

	memory_block& operator=(std::initializer_list<T> ilist);
public:
	Allocator get_allocator() const
	{
		return m_allocator;
	}
};