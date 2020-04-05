#version 460 core
#extension GL_ARB_bindless_texture : require

struct Material
{
	uvec2 base_idx;
	uvec2 gloss_idx;
	uvec2 normal_idx;
};

in vec3 v_pos;
in vec2 v_uv;
in flat Material v_material;
in mat3 v_TBN;

out vec4 f_color;

uniform sampler2DArray a_textures;

uniform vec3 a_light_pos;
uniform vec3 a_light_color;
uniform vec3 a_ambient;

vec3 calculate_directional_lighting(vec3 light_dir, vec3 light_color, vec3 normal, vec3 pos, float gloss)
{	
	float diffuse = max(dot(normal, light_dir), 0.0);

	vec3 view_dir = normalize(-pos);
	vec3 halfway_dir = normalize(light_dir + view_dir);  

	const float MAX_GLOSS = 256;
    float specular = pow(max(dot(normal, halfway_dir), 0.0), MAX_GLOSS * gloss);

	return light_color * (diffuse + specular);
}

vec3 calculate_point_lighting(vec3 light_pos, vec3 light_color, vec3 normal, vec3 pos, float gloss)
{
	vec3 light_dir = normalize(light_pos - v_pos);

	vec3 lighting = calculate_directional_lighting(light_dir, light_color, normal, pos, gloss);
	vec3 distance_vec = pos - light_pos;
	float attentuation = dot(distance_vec, distance_vec);
	return lighting / attentuation;
}

vec3 sample_normal_map()
{
	vec3 normal = texture(sampler2D(v_material.normal_idx), v_uv).xyz;
	normal = normal * 2 - 1;
	return normalize(v_TBN * normal);
}

void main()
{	
	vec4 base_color = texture(sampler2D(v_material.base_idx), v_uv);
	float gloss = texture(sampler2D(v_material.gloss_idx), v_uv).r;

	vec3 albedo = base_color.rgb;
	float alpha = base_color.a;

	vec3 normal = sample_normal_map();

	vec3 lighting = a_ambient + calculate_point_lighting(a_light_pos, a_light_color, normal, v_pos, gloss);

    f_color = vec4(albedo * lighting, alpha);
}