#pragma once

#include <list>
#include <functional>

template <typename... Args>
class Event
{
public:
	using handler_t = std::function<void(Args...)>;
private:
	using container_t = std::list<handler_t>;
private:
	container_t m_handlers;
public:
	using handler_id = typename container_t::iterator;
public:
	void operator()(Args... args)
	{
		for (const auto& handler : m_handlers)
		{
			handler(std::forward<Args>(args)...);
		}
	}
	handler_id operator+=(const handler_t& handler)
	{
		m_handlers.push_back(handler);
		return std::prev(m_handlers.end());
	}
	void operator-=(handler_id handler)
	{
		m_handlers.erase(handler);
	}
};