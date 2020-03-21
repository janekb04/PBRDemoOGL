#pragma once

#include <vector>
#include <list>
#include <initializer_list>
#include <type_traits>
#include <algorithm>

template <typename T, typename Container = std::vector<T>>
class unordered_array_set
{
private:
	Container m_container;
	using iter_list_t = std::list<int>;
	iter_list_t m_iterators;
	std::vector<iter_list_t::iterator> m_element_iterators;
private:
	template <typename It, typename Container>
	class _generic_iterator
	{
	private:
		It m_it;
		Container* m_container;

		It underlying_iterator()
		{
			return m_it;
		}
		friend class unordered_array_set<T, Container>;
	public:
		_generic_iterator() :
			m_it{},
			m_container{nullptr}
		{
		}

		_generic_iterator(It it, Container* p_container) :
			m_it{ it },
			m_container{ p_container }
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
			return (*m_container)[*m_it];
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
	using size_type = std::size_t;
	using value_type = T;
	using iterator = _generic_iterator<typename iter_list_t::iterator, Container>;
	using const_iterator = _generic_iterator<typename iter_list_t::const_iterator, const Container>;
	using reverse_iterator = _generic_iterator<typename iter_list_t::reverse_iterator, Container>;
	using const_reverse_iterator = _generic_iterator<typename iter_list_t::const_reverse_iterator, const Container>;
private:
	iterator _after_insert()
	{
		m_iterators.push_back(m_container.size() - 1);
		auto it = std::prev(m_iterators.end());
		m_element_iterators.push_back(it);
		return { it, &m_container };
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
		return { m_iterators.begin(), &m_container };
	}
	const_iterator begin() const noexcept
	{
		return { m_iterators.begin(), &m_container };
	}
	const_iterator cbegin() const noexcept
	{
		return { m_iterators.cbegin(), &m_container };
	}

	iterator end() noexcept
	{
		return { m_iterators.end(), &m_container };
	}
	const_iterator end() const noexcept
	{
		return { m_iterators.end(), &m_container };
	}
	const_iterator cend() const noexcept
	{
		return { m_iterators.cend(), &m_container };
	}

	reverse_iterator rbegin() noexcept
	{
		return { m_iterators.rbegin(), &m_container };
	}
	const_reverse_iterator rbegin() const noexcept
	{
		return { m_iterators.rbegin(), &m_container };
	}
	const_reverse_iterator crbegin() const noexcept
	{
		return { m_iterators.crbegin(), &m_container };
	}

	reverse_iterator rend() noexcept
	{
		return { m_iterators.rend(), &m_container };
	}
	const_reverse_iterator rend() const noexcept
	{
		return { m_iterators.rend(), &m_container };
	}
	const_reverse_iterator crend() const noexcept
	{
		return { m_iterators.crend(), &m_container };
	}

	bool empty() const noexcept
	{
		return m_container.empty();
	}

	size_type size() const noexcept
	{
		return m_container.size();
	}
	size_type max_size() const noexcept
	{
		return std::min(
			std::min(
				m_container.max_size(),
				m_iterators.max_size()
			),
			m_element_iterators.max_size()
		);
	}

	void clear() noexcept
	{
		m_container.clear();
		m_iterators.clear();
		m_element_iterators.clear();
	}

	iterator insert(const value_type& value)
	{
		m_container.push_back(value);
		return _after_insert();
	}
	iterator insert(value_type&& value)
	{
		m_container.push_back(std::move(value));
		return _after_insert();
	}

	template<typename... Args>
	iterator emplace(Args&&... args)
	{
		m_container.emplace_back(std::forward<Args>(args)...);
		return _after_insert();
	}

	iterator erase(iterator pos)
	{
		int i = *pos.underlying_iterator();

		m_container[i] = std::move(m_container[size() - 1]);
		*m_element_iterators[size() - 1] = *m_element_iterators[i];
		m_element_iterators[i] = std::move(m_element_iterators[size() - 1]);

		m_container.pop_back();
		m_element_iterators.pop_back();

		auto next = std::next(pos.underlying_iterator());
		m_iterators.erase(pos.underlying_iterator());

		return { next, &m_container };
	}

	iterator erase(iterator first, iterator last)
	{
		iterator result;
		for (iterator it = first; it != last; ++it)
			result = erase(it);
		return result;
	}

	void swap(unordered_array_set<T, Container>& other)
	{
		m_container.swap(other.m_container);
		m_iterators.swap(other.m_iterators);
		m_element_iterators.swap(other.m_element_iterators);
	}

	value_type* data() noexcept
	{
		return m_container.data();
	}
	const value_type* data() const noexcept
	{
		return m_container.data();
	}
};