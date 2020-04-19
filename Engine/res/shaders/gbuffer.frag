#version 460 core
#extension GL_ARB_bindless_texture : require

struct Material
{
	uvec2 albedo_idx;
	uvec2 normal_idx;
	uvec2 metalic_idx;
	uvec2 roughness_idx;
	uvec2 ao_idx;
};

in vec3 v_pos;
in vec2 v_uv;
in flat Material v_material;
in mat3 v_TBN;

layout (location = 0) out vec4 f_0;
layout (location = 1) out vec4 f_1;
layout (location = 2) out vec4 f_2;

vec3 sample_normal_map(vec2 uv, mat3 TBN)
{
	vec3 normal = texture(sampler2D(v_material.normal_idx), uv).xyz;
	normal = normal * 2 - 1;
	return normalize(TBN * normal);
}

void main()
{	
	vec2 uv = v_uv;
	
	vec4 base_color = texture(sampler2D(v_material.albedo_idx), uv);

	vec3 albedo = base_color.rgb;
	vec3 normal = sample_normal_map(uv, v_TBN);
	float metallic = texture(sampler2D(v_material.metalic_idx), uv).r;
	float roughness = texture(sampler2D(v_material.roughness_idx), uv).r;
	float ao = texture(sampler2D(v_material.ao_idx), uv).r;

	f_0 = vec4(v_pos, ao);
	f_1 = vec4(normal, metallic);
	f_2 = vec4(albedo, roughness);
}