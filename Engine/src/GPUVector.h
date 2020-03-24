//#pragma once
//
//#include "Vendor.h"
//#include "Utility.h"
//#include "Buffer.h"
//
//template <typename T>
//class GPUVector
//{
//public:
//	using value_type = T;
//	using size_type = std::size_t;
//	using difference_type = std::ptrdiff_t;
//	using reference = value_type&;
//	using const_reference = const value_type&;
//	using pointer = value_type*;
//	using const_pointer = const value_type*;
//	using iterator = ...;
//	using const_iterator = ...;
//	using reverse_iterator = ...;
//	using const_reverse_iterator = ...;
//private:
//	size_type m_size;
//	size_type m_capacity;
//	Buffer m_buffer;
//	pointer m_data_ptr;
//public:
//	GPUVector();
//	GPUVector(size_type count, const_reference value);
//	GPUVector(size_type count) :
//		GPUVector(count, T())
//	{
//	}
//	template< class InputIt >
//	GPUVector(InputIt first, InputIt last);
//	GPUVector(const GPUVector& other);
//	GPUVector(GPUVector&& other) noexcept;
//	GPUVector(std::initializer_list<T> init) :
//		GPUVector(init.begin(), init.end())
//	{
//	}
//	~GPUVector();
//
//	GPUVector& operator=(const GPUVector& other);
//	GPUVector& operator=(GPUVector&& other) noexcept;
//	GPUVector& operator=(std::initializer_list<T> init);
//
//	void assign(size_type count, const_reference value);
//	template<class InputIt>
//	void assign(InputIt first, InputIt last);
//	void assign(std::initializer_list<T> ilist);
//
//	reference at(size_type pos);
//	const_reference at(size_type pos) const;
//
//	reference operator[](size_type pos);
//	const_reference operator[](size_type pos) const;
//
//	reference front();
//	const_reference front() const;
//
//	reference back();
//	const_reference back() const;
//
//	T* data() noexcept;	
//	const T* data() const noexcept;
//
//	iterator begin() noexcept;
//	const_iterator begin() const noexcept;
//	const_iterator cbegin() const noexcept;
//
//	iterator end() noexcept;
//	const_iterator end() const noexcept;
//	const_iterator cend() const noexcept;
//
//	reverse_iterator rbegin() noexcept;
//	const_reverse_iterator rbegin() const noexcept;
//	const_reverse_iterator crbegin() const noexcept;
//
//	reverse_iterator rend() noexcept;
//	const_reverse_iterator rend() const noexcept;
//	const_reverse_iterator crend() const noexcept;
//
//	bool empty() const noexcept;
//
//	size_type size() const noexcept;
//
//	size_type max_size() const noexcept;
//
//	void reserve(size_type new_cap);
//
//	size_type capacity() const noexcept;
//
//	void shrink_to_fit();
//
//	void clear() noexcept;
//
//	iterator insert(const_iterator pos, const T& value);
//	iterator insert(const_iterator pos, T&& value);
//	iterator insert(const_iterator pos, size_type count, const T& value);
//	template< class InputIt >
//	void insert(iterator pos, InputIt first, InputIt last);
//	template< class InputIt >
//	iterator insert(const_iterator pos, InputIt first, InputIt last);
//	iterator insert(const_iterator pos, std::initializer_list<T> ilist);
//
//	template< class... Args >
//	iterator emplace(const_iterator pos, Args&&... args);
//
//	iterator erase(const_iterator pos);
//	iterator erase(const_iterator first, const_iterator last);
//
//	void push_back(const T& value);
//	void push_back(T&& value);
//
//	template< class... Args >
//	reference emplace_back(Args&&... args);
//
//	void pop_back();
//
//	void resize(size_type count);
//	void resize(size_type count, const value_type& value);
//
//	void swap(GPUVector& other) noexcept;
//};