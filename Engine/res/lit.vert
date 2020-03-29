#version 460 core

// -----------  Per vertex  -----------
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec3 a_tangent;
// ----------- Per instance -----------
layout (location = 4) in mat4 a_model;
//      location = 5
//      location = 6
//      location = 7
layout (location = 8) in int a_material_idx;
layout (location = 9) in mat3 a_normal_mat;
//      location = 10
//      location = 11

out vec3 v_pos;
out vec3 v_normal;
out vec3 v_tangent;
out vec2 v_uv;
out flat int v_base_idx;

uniform mat4 a_camera;
uniform mat4 a_view;

struct material
{
	int base_idx;
};

layout (std430, binding = 0) buffer materials
{
	material a_materials[];
};

void main()
{
	material mat = a_materials[a_material_idx];
	v_base_idx = mat.base_idx;
	
	vec4 pos_in_world_space = a_model * vec4(a_pos, 1);
	mat4 model_to_view_space = a_view * a_model;

	v_pos = vec3(model_to_view_space * pos_in_world_space);
	v_normal = vec3(a_view * vec4(a_normal_mat * a_normal, 0));
	v_tangent = vec3(model_to_view_space *  vec4(a_tangent, 0));
	v_uv = a_uv;

	gl_Position = a_camera * pos_in_world_space;
}