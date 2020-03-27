#pragma once

#include <vector>
#include <list>
#include <initializer_list>
#include <type_traits>
#include <algorithm>

template <typename T, typename Container = std::vector<T>>
class unordered_array_set
{
protected:
	Container c;
public:
	using container_type = Container;
	using size_type = std::size_t;
	using value_type = T;
	using reference = value_type&;
	using const_reference = const value_type&;
	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;
	using reverse_iterator = typename container_type::reverse_iterator;
	using const_reverse_iterator = typename container_type::const_reverse_iterator;
public:
	unordered_array_set() = default;
	unordered_array_set(const unordered_array_set& other) = default;
	unordered_array_set(unordered_array_set&& other) noexcept = default;
	unordered_array_set(std::initializer_list<T> init)
	{
		for (const T& value : init)
		{
			push_back(value);
		}
	}
	unordered_array_set(container_type&& container) :
		c{ std::move(container) }
	{
	}
	~unordered_array_set() = default;

	unordered_array_set& operator=(const unordered_array_set& other) = default;
	unordered_array_set& operator=(unordered_array_set&& other) noexcept = default;

	iterator begin() noexcept
	{
		return c.begin();
	}
	const_iterator begin() const noexcept
	{
		return c.begin();
	}
	const_iterator cbegin() const noexcept
	{
		return c.cbegin();
	}

	iterator end() noexcept
	{
		return c.end();
	}
	const_iterator end() const noexcept
	{
		return c.end();
	}
	const_iterator cend() const noexcept
	{
		return c.cend();
	}

	reverse_iterator rbegin() noexcept
	{
		return c.rbegin();
	}
	const_reverse_iterator rbegin() const noexcept
	{
		return c.rbegin();
	}
	const_reverse_iterator crbegin() const noexcept
	{
		return c.crbegin();
	}

	reverse_iterator rend() noexcept
	{
		return c.rend();
	}
	const_reverse_iterator rend() const noexcept
	{
		return c.rend();
	}
	const_reverse_iterator crend() const noexcept
	{
		return c.crend();
	}

	reference front()
	{
		return *begin();
	}
	const_reference front() const
	{
		return *cbegin();
	}

	reference back()
	{
		return *std::prev(end());
	}
	const_reference back() const
	{
		return *std::prev(cend());
	}

	bool empty() const noexcept
	{
		return c.empty();
	}

	size_type size() const noexcept
	{
		return c.size();
	}
	size_type max_size() const noexcept
	{
		return c.max_size();
	}

	void clear() noexcept
	{
		c.clear();
	}

	iterator insert(const value_type& value)
	{
		c.push_back(value);
		return std::prev(end());
	}
	iterator insert(value_type&& value)
	{
		c.push_back(std::move(value));
		return std::prev(end());
	}

	template<typename... Args>
	iterator emplace(Args&&... args)
	{
		c.emplace_back(std::forward<Args>(args)...);
		return std::prev(end());
	}

	iterator erase(iterator pos)
	{
		*pos = std::move(c.back());
		c.pop_back();
		return std::next(pos);
	}

	iterator erase(iterator first, iterator last)
	{
		iterator result;
		for (iterator it = first; it != last; ++it)
			result = erase(it);
		return result;
	}

	void push_back(const T& value)
	{
		insert(value);
	}

	void push_back(T&& value)
	{
		insert(std::move(value));
	}

	template<typename... Args>
	reference emplace_back(Args&&... args)
	{
		return *emplace(std::forward<Args>(args)...);
	}

	void pop_back()
	{
		erase(std::prev(end()));
	}

	void swap(unordered_array_set<T, Container>& other)
	{
		c.swap(other.c);
	}

	value_type* data() noexcept
	{
		return c.data();
	}
	const value_type* data() const noexcept
	{
		return c.data();
	}
};