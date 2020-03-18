#pragma once

#include "Vendor.h"

class Transform
{
private:
	glm::quat orientation;
	glm::vec3 scale;
	glm::mat4 trans;

private:
	static inline void make_trans_matrix(const glm::vec3& pos, glm::mat4& trans)
	{
		trans[3] = glm::vec4{ pos, 1.0f };
	}
	static inline void make_rot_matrix(const glm::quat& rot, const glm::vec3& scale, glm::mat4& trans)
	{
		glm::mat3 rotation = glm::toMat3(rot);
		trans[0] = glm::vec4{ rotation[0], 0.0f } * scale.x;
		trans[1] = glm::vec4{ rotation[1], 0.0f } * scale.y;
		trans[2] = glm::vec4{ rotation[2], 0.0f } * scale.z;
	}
	static inline glm::mat4 make_matrix(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scl)
	{
		glm::mat4 trans;
		make_rot_matrix(rot, scl, trans);
		make_trans_matrix(pos, trans);
		return trans;
	}

public:
	constexpr inline Transform() :
		orientation( 0, 0, 0, 1 ),
		scale( 1, 1, 1 ),
		trans( 1 )
	{
	}

	inline Transform(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scl) :
		orientation(rot),
		scale(scl),
		trans(make_matrix(pos, rot, scl))
	{
	}

	inline Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl) :
		Transform(pos, glm::quat(rot), scl)
	{
	}

public:
	glm::mat4 to_mat4() const
	{
		return trans;
	}
	glm::vec3 get_position() const
	{
		return trans[3];
	}
	void set_position(const glm::vec3 pos)
	{
		make_trans_matrix(pos, trans);
	}
	glm::quat get_orientation() const
	{
		return orientation;
	}
	void set_orientation(const glm::quat& rot)
	{
		orientation = rot;
		make_rot_matrix(orientation, scale, trans);
	}
	void set_orientation(const glm::vec3& euler)
	{
		set_orientation(glm::quat(euler));
	}
	glm::vec3 get_scale() const
	{
		return scale;
	}
	void set_scale(const glm::vec3& scl)
	{
		glm::vec3 factor = scl / scale;
		scale = scl;
		trans[0] *= factor.x;
		trans[1] *= factor.y;
		trans[2] *= factor.z;
	}
	void translate(const glm::vec3& translation)
	{
		set_position(get_position() + translation);
	}
	void rotate(const glm::quat& rotation)
	{
		set_orientation(get_orientation() * rotation);
	}
public:
	static const Transform ORIGIN;
};

inline const Transform Transform::ORIGIN{};