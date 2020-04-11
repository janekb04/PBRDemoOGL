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
	uvec2 albedo_idx;
	uvec2 normal_idx;
	uvec2 metalic_idx;
	uvec2 roughness_idx;
	uvec2 ao_idx;
};

out vec3 v_pos;
out vec2 v_uv;
out flat Material v_material;
out mat3 v_TBN;

uniform mat4 a_camera;

layout (std430, binding = 0) restrict readonly buffer materials
{
	uvec2 a_materials[];
};

void send_material_properties()
{
	int i = a_material_idx * 5;

	v_material.albedo_idx = a_materials[i];
	v_material.normal_idx = a_materials[i + 1];
	v_material.metalic_idx = a_materials[i + 2];
	v_material.roughness_idx = a_materials[i + 3];
	v_material.ao_idx = a_materials[i + 4];
}

void send_vertex_properties()
{
	v_pos = vec3(a_model * vec4(a_pos, 1));
	v_uv = a_uv;

	gl_Position = a_camera * vec4(v_pos, 1);
}

void send_tangent_space_matrix()
{
	vec3 normal = normalize(a_normal_mat * a_normal);
	vec3 tangent = normalize(a_normal_mat * a_tangent);
	vec3 bitangent = cross(normal, tangent);

	v_TBN = mat3(tangent, bitangent, normal);
}

void main()
{
	send_material_properties();
	send_vertex_properties();
	send_tangent_space_matrix();
}