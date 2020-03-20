#version 460 core

in vec2 v_uv;
in flat int v_base_idx;

out vec4 f_color;

uniform sampler2DArray a_textures;

void main()
{	
	f_color = texture(a_textures, vec3(v_uv, v_base_idx));
}