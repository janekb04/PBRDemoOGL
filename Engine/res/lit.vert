#version 460 core

layout (location = 0) in vec3 a_pos;
//layout (location = 1) in vec3 a_normal;
//layout (location = 2) in vec2 a_uv;
//layout (location = 3) in vec3 a_tangent;

out vec4 v_color;

uniform mat4 a_camera;

void main()
{
	gl_Position = a_camera * vec4(a_pos, 1);
}