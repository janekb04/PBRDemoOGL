#version 460 core
#extension GL_ARB_bindless_texture : require

struct Material
{
	uvec2 base_idx;
	uvec2 gloss_idx;
	uvec2 normal_idx;
};

struct PointLight
{
	vec3 pos;
	vec3 color;
};
struct DirectionalLight
{
	vec3 direction;
	vec3 color;
};
struct Spotlight
{
	vec3 pos;
	vec3 direction;
	vec3 color;
	float cos_difference;
	float cos_outer_angle;
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

float f_lit(vec3 light_dir, vec3 normal, vec3 view_dir, float gloss)
{
	float diffuse = clamp(dot(normal, light_dir), 0.0, 1.0);

	vec3 halfway_dir = normalize(light_dir + view_dir);  

	const float MAX_GLOSS = 256;
    float specular = pow(clamp(dot(normal, halfway_dir), 0.0, 1.0), MAX_GLOSS * gloss);

	return diffuse + specular;
}

float f_attentuation(vec3 r)
{
	float attentuation = dot(r, r);
	return 1 / attentuation;
}

float f_dir(float t)
{
	return t * t;
}

vec3 calculate_directional_lighting(DirectionalLight light, vec3 normal, vec3 view_dir, float gloss)
{	
	return light.color * f_lit(light.direction, normal, view_dir, gloss);
}

vec3 calculate_point_lighting(PointLight light, vec3 normal, vec3 pos, vec3 view_dir, float gloss)
{
	vec3 r = light.pos - pos;
	vec3 light_dir = normalize(r);

	return light.color * f_attentuation(r) * f_lit(light_dir, normal, view_dir, gloss);
}

vec3 calculate_spotlight_lighting(Spotlight light, vec3 normal, vec3 pos, vec3 view_dir, float gloss)
{
	vec3 r = light.pos - pos;
	vec3 light_dir = normalize(r);

	float cos_angle = dot(light_dir, light.direction);
	float t = (cos_angle - light.cos_outer_angle) / light.cos_difference;

	return light.color * f_dir(t) * f_attentuation(r) * f_lit(light_dir, normal, view_dir, gloss);
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

	vec3 lighting = a_ambient + calculate_point_lighting(PointLight(a_light_pos, a_light_color), normal, v_pos, normalize(-v_pos), gloss);

    f_color = vec4(albedo * lighting, alpha);
}