#version 460 core

in vec3 v_pos;
in vec3 v_normal;
in vec3 v_tangent;
in vec2 v_uv;
in flat int v_base_idx;

out vec4 f_color;

uniform sampler2DArray a_textures;

uniform vec3 a_light_dir;
uniform vec3 a_light_color;
uniform vec3 a_ambient;

vec3 calculate_directional_lighting(vec3 light_dir, vec3 light_color, vec3 normal)
{	
	float diffuse = max(dot(normal, light_dir), 0.0);

	float specular = 0;

	return light_color * (diffuse + specular);
}

vec3 calculate_point_lighting(vec3 light_pos, vec3 light_color, vec3 normal)
{
	vec3 light_dir = normalize(light_pos - v_pos);
	return calculate_directional_lighting(light_dir, light_color, normal);
}

void main()
{	
	vec4 base_color = texture(a_textures, vec3(v_uv, v_base_idx));

	vec3 albedo = base_color.rgb;
	float alpha = base_color.a;

	vec3 normal = normalize(v_normal);

	//vec3 lighting = a_ambient + calculate_point_lighting(a_light_pos, a_light_color, normal);
	vec3 lighting = a_ambient + calculate_directional_lighting(a_light_dir, a_light_color, normal);

    f_color = vec4(albedo * lighting, alpha);
}