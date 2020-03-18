#version 460 core

in vec3 a_pos;
//in vec3 a_normal;
//in vec2 a_uv;
//in vec3 a_tangent;
in mat4 a_model;

out vec4 v_color;

uniform mat4 a_camera;

void main()
{
	gl_Position = a_camera * a_model * vec4(a_pos, 1);
}