#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in mat4 a_model;

out vec4 v_color;

uniform mat4 a_camera;

void main()
{
	gl_Position = a_camera * a_model * vec4(a_pos, 1);
	v_color = vec4(gl_DrawID, 0, 0, 1);
}