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

out vec2 v_uv;
out flat int v_base_idx;

uniform mat4 a_camera;

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
	gl_Position = a_camera * a_model * vec4(a_pos, 1);

	material mat = a_materials[a_material_idx];
	
	v_base_idx = mat.base_idx;
	v_uv = a_uv;
}