#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec3 a_tangent;

out vec4 v_color;

void main()
{
	gl_Position = vec4(a_pos, 1);
	if (gl_DrawID == 0)
		v_color = vec4(1, 0, 0, 1);
	else
		v_color = vec4(0, 1, 0, 1);
}