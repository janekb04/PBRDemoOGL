#pragma once

#include "Vendor.h"
#include "Viewport.h"
#include "Transform.h"

struct IViewer
{
	virtual glm::mat4 get_projection_matrix(const Viewport&) const = 0;
	virtual glm::mat4 get_view_matrix(const Transform&) const = 0;
	virtual ~IViewer() {}
};

struct PerspectiveViewer : public IViewer
{
	float fov = 60.0f;
	float nearPlane = 0.3f;
	float farPlane = 1000.0f;

	virtual glm::mat4 get_projection_matrix(const Viewport& viewport) const override
	{
		if (farPlane == std::numeric_limits<float>::infinity())
			return glm::infinitePerspective(fov, viewport.aspect<float>(), farPlane);
		else
			return glm::perspective(glm::radians(fov), viewport.aspect<float>(), nearPlane, farPlane);
	}
	virtual glm::mat4 get_view_matrix(const Transform& transform) const override
	{
		return glm::inverse(transform.to_mat4());
	}
};