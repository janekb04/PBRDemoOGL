#pragma once

#include "Vendor.h"
#include "Viewport.h"
#include "Window.h"
#include "Transform.h"

//class PerspectiveCamera
//{
//public:
//	Transform transform;
//	float fov = 45.0f;
//	float nearPlane = 0.3f;
//	float farPlane = 1000.0f;
//
//	glm::mat4 get_projection_matrix(const Viewport& viewport) const
//	{
//		if (farPlane == std::numeric_limits<float>::infinity())
//			return glm::infinitePerspective(fov, viewport.aspect<float>(), farPlane);
//		else
//			return glm::perspective(glm::radians(fov), viewport.aspect<float>(), nearPlane, farPlane);
//	}
//	glm::mat4 get_view_matrix() const
//	{
//		return transform.to_mat4();
//	}
//};

struct Camera
{
public:
	Viewport viewport;
	Transform transform;
public:
	Camera(const Viewport& viewport = {}) :
		viewport(viewport)
	{
	}
public:
	void render()
	{
		glViewport(viewport.offset.x, viewport.offset.y, viewport.size.x, viewport.size.y);

		//render_scene();
	}
};
