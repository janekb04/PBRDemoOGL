#version 460 core
#extension GL_ARB_bindless_texture : enable

in vec2 v_uv;
out vec4 f_color;

uniform uvec2 a_input_idx;

void main()
{
	f_color = texture(sampler2D(a_input_idx), v_uv);
}