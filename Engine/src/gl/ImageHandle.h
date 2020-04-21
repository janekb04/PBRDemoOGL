#pragma once

#include "../Vendor.h"

class ImageHandle
{
private:
	GLuint64 handle;
public:
	ImageHandle() :
		handle(0)
	{
	}

	explicit ImageHandle(GLuint64 handle) :
		handle(handle)
	{
	}

	operator glm::uvec2() const
	{
		glm::uvec2 result;
		*reinterpret_cast<GLuint64*>(&result) = handle;
		return result;
	}
	operator GLuint64() const
	{
		return handle;
	}

	void make_resident(GLenum access)
	{
		glMakeImageHandleResidentARB(handle, access);
	}
	void make_non_resident()
	{
		glMakeImageHandleNonResidentARB(handle);
	}
};