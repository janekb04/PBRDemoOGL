#version 460 core
#extension GL_ARB_bindless_texture : require

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

struct Material
{
	uvec2 base_idx;
	uvec2 gloss_idx;
	uvec2 normal_idx;
};

out vec3 v_pos;
out vec2 v_uv;
out flat Material v_material;
out mat3 v_TBN;

uniform mat4 a_camera;
uniform mat4 a_view;

layout (std430, binding = 0) buffer materials
{
	uvec2 a_materials[];
};

void send_material_properties()
{
	int i = a_material_idx * 3;

	v_material.base_idx = a_materials[i];
	v_material.gloss_idx = a_materials[i + 1];
	v_material.normal_idx = a_materials[i + 2];
}

void send_vertex_properties()
{
	vec4 pos_in_world_space = a_model * vec4(a_pos, 1);
	mat4 model_view = a_view * a_model;

	v_pos = vec3(model_view * pos_in_world_space);
	v_uv = a_uv;

	gl_Position = a_camera * pos_in_world_space;
}

void send_tangent_space_matrix()
{
	mat3 normal_view = mat3(a_view) * a_normal_mat;

	vec3 normal = normalize(normal_view * a_normal);
	vec3 tangent = normalize(normal_view * a_tangent);
	vec3 bitangent = cross(normal, tangent);

	v_TBN = mat3(tangent, bitangent, normal);
}

void main()
{
	send_material_properties();
	send_vertex_properties();
	send_tangent_space_matrix();
}