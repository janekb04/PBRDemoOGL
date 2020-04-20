#pragma once

#include "../Vendor.h"

template <typename Helper>
class GLObject
{
protected:
	GLuint handle;
private:
	static GLuint create_handle()
	{
		GLuint handle;
		Helper::create(1, &handle);
		return handle;
	}
	static void destroy_handle(GLuint handle)
	{
		if (handle != 0)
			Helper::destroy(1, &handle);
	}
public:
	GLObject() :
		handle{ create_handle() }
	{
	}

	GLObject(const GLObject&) = delete;
	GLObject(GLObject&& other) noexcept :
		handle{ other.handle }
	{
		other.handle = 0;
	}

	GLObject& operator=(const GLObject&) = delete;
	GLObject& operator=(GLObject&& other) noexcept
	{
		destroy_handle(handle);
		handle = other.handle;
		other.handle = 0;

		return *this;
	};

	operator GLuint() const noexcept
	{
		return handle;
	}

	operator bool() const noexcept
	{
		return handle != 0;
	}

	~GLObject()
	{
		destroy_handle(handle);
	}
};