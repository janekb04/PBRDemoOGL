#pragma once

#include "Vendor.h"

class TextureHandle
{
private:
	GLuint64 handle;
public:
	TextureHandle() :
		handle(0)
	{
	}

	explicit TextureHandle(GLuint64 handle) :
		handle(handle)
	{
	}

	void make_resident()
	{
		glMakeTextureHandleResidentARB(handle);
	}
	void make_non_resident()
	{
		glMakeTextureHandleNonResidentARB(handle);
	}
	GLuint64 raw() const
	{
		return handle;
	}
};