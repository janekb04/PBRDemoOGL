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
private:
	using iter_list_t = std::list<int>;
	iter_list_t m_iterators;
	std::vector<iter_list_t::iterator> m_element_iterators;
private:
	template <typename It, typename Container>
	class _generic_iterator
	{
	private:
		It m_it;
		Container* c;

		It underlying_iterator()
		{
			return m_it;
		}
		friend class unordered_array_set<T, Container>;
	public:
		using difference_type = size_t;
		using value_type = It::value_type;
		using pointer = It::pointer;
		using reference = It::reference;
		using iterator_category = std::bidirectional_iterator_tag;
	public:
		_generic_iterator() :
			m_it{},
			c{nullptr}
		{
		}

		_generic_iterator(It it, Container* p_container) :
			m_it{ it },
			c{ p_container }
		{
		}

		_generic_iterator& operator++() noexcept
		{
			++m_it;
			return *this;
		}
		_generic_iterator operator++(int) noexcept
		{
			_generic_iterator it = *this;
			++(*this);
			return it;
		}

		_generic_iterator& operator--() noexcept
		{
			--m_it;
			return *this;
		}
		_generic_iterator operator--(int) noexcept
		{
			_generic_iterator it = *this;
			--(*this);
			return it;
		}

		typename Container::reference& operator*() const noexcept
		{
			return (*c)[*m_it];
		}

		typename Container::pointer operator->() const noexcept
		{
			return &(**this);
		}

		bool operator==(const _generic_iterator<It, Container> other) const noexcept
		{
			return m_it == other.m_it;
		}
		bool operator!=(const _generic_iterator<It, Container> other) const noexcept
		{
			return !((*this) == other);
		}
	};
public:
	using container_type = Container;
	using size_type = std::size_t;
	using value_type = T;
	using iterator = _generic_iterator<typename iter_list_t::iterator, Container>;
	using const_iterator = _generic_iterator<typename iter_list_t::const_iterator, const Container>;
	using reverse_iterator = _generic_iterator<typename iter_list_t::reverse_iterator, Container>;
	using const_reverse_iterator = _generic_iterator<typename iter_list_t::const_reverse_iterator, const Container>;
private:
	iterator _after_insert()
	{
		m_iterators.push_back(c.size() - 1);
		auto it = std::prev(m_iterators.end());
		m_element_iterators.push_back(it);
		return { it, &c };
	}
public:
	unordered_array_set() = default;
	//unordered_array_set(const unordered_array_set<T, Container>& other) = default;
	unordered_array_set(unordered_array_set<T, Container>&& other) noexcept = default;
	unordered_array_set(std::initializer_list<T> init)
	{
		for (const T& value : init)
		{
			insert(value);
		}
	}
	~unordered_array_set() = default;

	//unordered_array_set& operator=(const unordered_array_set<T, Container>& other) = default;
	unordered_array_set& operator=(unordered_array_set<T, Container>&& other) noexcept = default;

	iterator begin() noexcept
	{
		return { m_iterators.begin(), &c };
	}
	const_iterator begin() const noexcept
	{
		return { m_iterators.begin(), &c };
	}
	const_iterator cbegin() const noexcept
	{
		return { m_iterators.cbegin(), &c };
	}

	iterator end() noexcept
	{
		return { m_iterators.end(), &c };
	}
	const_iterator end() const noexcept
	{
		return { m_iterators.end(), &c };
	}
	const_iterator cend() const noexcept
	{
		return { m_iterators.cend(), &c };
	}

	reverse_iterator rbegin() noexcept
	{
		return { m_iterators.rbegin(), &c };
	}
	const_reverse_iterator rbegin() const noexcept
	{
		return { m_iterators.rbegin(), &c };
	}
	const_reverse_iterator crbegin() const noexcept
	{
		return { m_iterators.crbegin(), &c };
	}

	reverse_iterator rend() noexcept
	{
		return { m_iterators.rend(), &c };
	}
	const_reverse_iterator rend() const noexcept
	{
		return { m_iterators.rend(), &c };
	}
	const_reverse_iterator crend() const noexcept
	{
		return { m_iterators.crend(), &c };
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
		return std::min(
			std::min(
				c.max_size(),
				m_iterators.max_size()
			),
			m_element_iterators.max_size()
		);
	}

	void clear() noexcept
	{
		c.clear();
		m_iterators.clear();
		m_element_iterators.clear();
	}

	iterator insert(const value_type& value)
	{
		c.push_back(value);
		return _after_insert();
	}
	iterator insert(value_type&& value)
	{
		c.push_back(std::move(value));
		return _after_insert();
	}

	template<typename... Args>
	iterator emplace(Args&&... args)
	{
		c.emplace_back(std::forward<Args>(args)...);
		return _after_insert();
	}

	iterator erase(iterator pos)
	{
		int i = *pos.underlying_iterator();

		c[i] = std::move(c[size() - 1]);
		*m_element_iterators[size() - 1] = *m_element_iterators[i];
		m_element_iterators[i] = std::move(m_element_iterators[size() - 1]);

		c.pop_back();
		m_element_iterators.pop_back();

		auto next = std::next(pos.underlying_iterator());
		m_iterators.erase(pos.underlying_iterator());

		return { next, &c };
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
		m_iterators.swap(other.m_iterators);
		m_element_iterators.swap(other.m_element_iterators);
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