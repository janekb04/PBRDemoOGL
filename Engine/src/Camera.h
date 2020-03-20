#pragma once

#include "Vendor.h"
#include "Viewport.h"
#include "Transform.h"

struct PerspectiveCamera
{
	Transform transform;
	float fov = 60.0f;
	float nearPlane = 0.3f;
	float farPlane = 1000.0f;

	glm::mat4 get_projection_matrix(const Viewport& viewport) const
	{
		if (farPlane == std::numeric_limits<float>::infinity())
			return glm::infinitePerspective(fov, viewport.aspect<float>(), farPlane);
		else
			return glm::perspective(glm::radians(fov), viewport.aspect<float>(), nearPlane, farPlane);
	}
	glm::mat4 get_view_matrix() const
	{
		return glm::inverse(transform.to_mat4());
	}
};