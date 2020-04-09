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
	vec4 pos;
	vec4 color;
};
struct DirectionalLight
{
	vec4 direction;
	vec4 color;
};
struct Spotlight
{
	vec4 pos;
	vec4 direction;
	vec4 color;
	float cos_difference;
	float cos_outer_angle;
};

in vec3 v_pos;
in vec2 v_uv;
in flat Material v_material;
in mat3 v_TBN;

out vec4 f_color;

uniform sampler2DArray a_textures;
uniform vec3 a_camera_pos;

uniform vec3 a_ambient;

layout (std430, binding = 1)  restrict readonly buffer point_lights
{
	PointLight a_point_lights[];
};
layout (std430, binding = 2)  restrict readonly buffer directional_lights
{
	DirectionalLight a_directional_lights[];
};
layout (std430, binding = 3)  restrict readonly buffer spotlights
{
	Spotlight a_spotlights[];
};

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
	return light.color.rgb * f_lit(light.direction.xyz, normal, view_dir, gloss);
}

vec3 calculate_point_lighting(PointLight light, vec3 normal, vec3 pos, vec3 view_dir, float gloss)
{
	vec3 r = light.pos.xyz - pos;
	vec3 light_dir = normalize(r);

	return light.color.rgb * f_attentuation(r) * f_lit(light_dir, normal, view_dir, gloss);
}

vec3 calculate_spotlight_lighting(Spotlight light, vec3 normal, vec3 pos, vec3 view_dir, float gloss)
{
	vec3 r = light.pos.xyz - pos;
	vec3 light_dir = normalize(r);

	float cos_angle = dot(light_dir, light.direction.xyz);
	float t = (cos_angle - light.cos_outer_angle) / light.cos_difference;

	return light.color.rgb * f_dir(t) * f_attentuation(r) * f_lit(light_dir, normal, view_dir, gloss);
}

vec3 sample_normal_map()
{
	vec3 normal = texture(sampler2D(v_material.normal_idx), v_uv).xyz;
	normal = normal * 2 - 1;
	return normalize(v_TBN * normal);
}

vec3 RRTAndODTFit(vec3 v)
{
    vec3 a = v * (v + 0.0245786f) - 0.000090537f;
    vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

vec3 ACESFitted(vec3 color)
{
	//from https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl
    const mat3 ACESInputMat =
	{
		{0.59719, 0.35458, 0.04823},
		{0.07600, 0.90834, 0.01566},
		{0.02840, 0.13383, 0.83777}
	};
	const mat3 ACESOutputMat =
	{
		{ 1.60475, -0.53108, -0.07367},
		{-0.10208,  1.10813, -0.00605},
		{-0.00327, -0.07276,  1.07602}
	};

	color = ACESInputMat * color;
    color = RRTAndODTFit(color);
    color = ACESOutputMat * color;
    color = clamp(color, 0, 1);
    return color;
}

vec3 reinhard(vec3 color)
{
	return color / (color + 1);
}

vec3 tonemapper(vec3 color)
{
	//return ACESFitted(color);
	return reinhard(color);
}

void main()
{	
	vec4 base_color = texture(sampler2D(v_material.base_idx), v_uv);
	float gloss = texture(sampler2D(v_material.gloss_idx), v_uv).r;

	vec3 albedo = base_color.rgb;
	float alpha = base_color.a;

	vec3 normal = sample_normal_map();

	vec3 view_dir = normalize(a_camera_pos - v_pos);

	vec3 lighting = a_ambient;
	for (int i = 0; i < a_point_lights.length(); ++i)
	{
		lighting += calculate_point_lighting(a_point_lights[i], normal, v_pos, view_dir, gloss);
	}
	for (int i = 0; i < a_directional_lights.length(); ++i)
	{
		lighting += calculate_directional_lighting(a_directional_lights[i], normal, view_dir, gloss);
	}
	for (int i = 0; i < a_spotlights.length(); ++i)
	{
		lighting += calculate_spotlight_lighting(a_spotlights[i], normal, v_pos, view_dir, gloss);
	}

    f_color = vec4(tonemapper(albedo * lighting), alpha);
}