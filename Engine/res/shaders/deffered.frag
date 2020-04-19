#version 460 core
#extension GL_ARB_bindless_texture : require

struct Fragment
{
	vec3 pos;
	vec3 albedo;
	vec3 normal;
	float metallic;
	float roughness;
	float ao;
};

struct Material
{
	uvec2 albedo_idx;
	uvec2 normal_idx;
	uvec2 metalic_idx;
	uvec2 roughness_idx;
	uvec2 ao_idx;
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

out vec4 f_color;

uniform sampler2DArray a_gbuffer;
uniform vec3 a_camera_pos;
uniform vec3 a_ambient;

const float PI = 3.14159265359;

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

//Normal distribution function: Trowbridge-Reitz GGX
float NDF(float n_dot_h, float roughness)
{
	float roughness_sqr = roughness * roughness;

	float temp = n_dot_h * n_dot_h * (roughness_sqr - 1) + 1;

	return roughness_sqr / (PI * temp * temp);
}

//Geometry function: Schlick-GGX
float G_Schlick(float n_dot_v, float k)
{
	return n_dot_v / (n_dot_v * (1 - k) + k);
}

float k_direct(float roughness)
{
	float roughness_plus_one = roughness + 1;
	float k = roughness_plus_one * roughness_plus_one / 8;
	return k;
}

float k_IBL(float roughness)
{
	float k = roughness * roughness / 2;
	return k;
}

//Geometry function: Smith's method
float G_Smith(float n_dot_v, float n_dot_l, float k)
{
	return G_Schlick(n_dot_v, k) * G_Schlick(n_dot_l, k);
}

//Fresnel equation: Schlick approximation
vec3 F_Schlick(float h_dot_v, vec3 f0)
{
	float temp = 1 - h_dot_v;
	float temp2 = temp * temp;
	float temp5 = temp2 * temp2 * temp;

	return f0 + (1 - f0) * temp5;
}

vec3 F(float h_dot_v, vec3 albedo, float metallic)
{
	const float DIELECTRIC_BASE_FRESNEL = 0.04;
	vec3 f0 = mix(vec3(DIELECTRIC_BASE_FRESNEL), albedo, metallic);
	return F_Schlick(h_dot_v, f0);
}

//Lighting using Cook-Torrance BRDF
vec3 f_lit(vec3 light_dir, vec3 light_radiance, vec3 normal, vec3 view_dir, vec3 albedo, float metallic, float roughness)
{
	vec3 halfway = normalize(view_dir + light_dir);
	float n_dot_v = clamp(dot(normal, view_dir), 0, 1);
	float n_dot_l = clamp(dot(normal, light_dir), 0, 1);
	float h_dot_v = clamp(dot(halfway, view_dir), 0, 1);
	float n_dot_h = clamp(dot(normal, halfway), 0, 1);

	float ndf = NDF(n_dot_h, roughness);
	float g = G_Smith(n_dot_v, n_dot_l, k_direct(roughness));
	vec3 f = F(h_dot_v, albedo, metallic);
	float normalization_factor = 4 * n_dot_v * n_dot_l;

	vec3 kS = f;
	vec3 kD = (vec3(1) - kS) * (1 - metallic);

	vec3 specular = ndf * g * f / max(normalization_factor, 0.001);
	vec3 diffuse = kD * albedo / PI;

	vec3 BRDF = diffuse + specular;

	return BRDF * light_radiance * n_dot_l;
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

vec3 calculate_directional_lighting(DirectionalLight light, vec3 normal, vec3 view_dir, vec3 albedo, float metallic, float roughness)
{	
	return f_lit(light.direction.xyz, light.color.rgb, normal, view_dir, albedo, metallic, roughness);
}

vec3 calculate_point_lighting(PointLight light, vec3 normal, vec3 pos, vec3 view_dir, vec3 albedo, float metallic, float roughness)
{
	vec3 r = light.pos.xyz - pos;
	vec3 light_dir = normalize(r);

	return f_attentuation(r) * f_lit(light_dir, light.color.rgb, normal, view_dir, albedo, metallic, roughness);
}

vec3 calculate_spotlight_lighting(Spotlight light, vec3 normal, vec3 pos, vec3 view_dir, vec3 albedo, float metallic, float roughness)
{
	vec3 r = light.pos.xyz - pos;
	vec3 light_dir = normalize(r);

	float cos_angle = dot(light_dir, light.direction.xyz);
	float t = clamp((cos_angle - light.cos_outer_angle) / light.cos_difference, 0, 1);

	return f_dir(t) * f_attentuation(r) * f_lit(light_dir, light.color.rgb, normal, view_dir, albedo, metallic, roughness);
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

	color = color * ACESInputMat;
    color = RRTAndODTFit(color);
    color = color * ACESOutputMat;
    color = clamp(color, 0, 1);
    return color;
}

vec3 RGBtoSRGB(vec3 color)
{
	return pow(color, vec3(1/2.2));
}

vec3 SRGBtoRGB(vec3 color)
{
	return pow(color, vec3(2.2));
}

vec3 reinhard(vec3 color)
{
	vec3 tonemapped = color / (color + 1);
	return RGBtoSRGB(tonemapped);
}

vec3 tonemapper(vec3 color)
{
	return ACESFitted(RGBtoSRGB(color));
	//return reinhard(color);
}

Fragment get_fragment_data_from_gbuffer(vec2 uv)
{	
	vec4 v0 = texture(a_gbuffer, vec3(uv, 0));
	vec4 v1 = texture(a_gbuffer, vec3(uv, 1));
	vec4 v2 = texture(a_gbuffer, vec3(uv, 2));

	return Fragment(
		v0.xyz,
		v2.xyz,
		v1.xyz,
		v1.w,
		v2.w,
		v0.w
	);
}

void main()
{	
	Fragment data = get_fragment_data_from_gbuffer();

	vec3 view_dir = normalize(a_camera_pos - data.pos);

	vec3 lighting = a_ambient * data.ao * data.albedo;
	for (int i = 0; i < a_point_lights.length(); ++i)
	{
		lighting += calculate_point_lighting(a_point_lights[i], data.normal, data.pos, view_dir, data.albedo, data.metallic, data.roughness);
	}
	for (int i = 0; i < a_directional_lights.length(); ++i)
	{
		lighting += calculate_directional_lighting(a_directional_lights[i], data.normal, view_dir, data.albedo, data.metallic, data.roughness);
	}
	for (int i = 0; i < a_spotlights.length(); ++i)
	{
		lighting += calculate_spotlight_lighting(a_spotlights[i], data.normal, data.pos, view_dir, data.albedo, data.metallic, data.roughness);
	}

    f_color = vec4(tonemapper(lighting), 1);
}