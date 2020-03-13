#pragma once

#include "Vendor.h"

struct Viewport
{
public:
	glm::uvec2 offset;
	glm::uvec2 size;
public:
	template <typename FloatT>
	FloatT aspect() const
	{
		return FloatT(size.x) / FloatT(size.y);
	}
};