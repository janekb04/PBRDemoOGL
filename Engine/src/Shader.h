#pragma once

#include "Vendor.h"
#include "VertexArray.h"

class Shader
{
private:
	GLuint handle;
	friend class ShaderProgram;

	void check_for_errors()
	{
		int success;
		std::string infoLog;
		infoLog.resize(BUFSIZ);

		glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(handle, BUFSIZ, NULL, infoLog.data());
			throw std::runtime_error("shader compilation failed: " + infoLog);
		}
	}
public:
	Shader(const char* const source, GLenum type) :
		handle(glCreateShader(type))
	{
		auto src_cpy = source;
		glShaderSource(handle, 1, &src_cpy, 0);
		glCompileShader(handle);

		check_for_errors();
	}

	~Shader()
	{
		glDeleteShader(handle);
	}
};

class ShaderProgram
{
private:
	GLuint handle;

	void check_for_errors()
	{
		int success;
		std::string infoLog;
		infoLog.resize(BUFSIZ);

		glGetProgramiv(handle, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(handle, BUFSIZ, NULL, infoLog.data());
			throw std::runtime_error("shader program linkage failed: " + infoLog);
		}
	}
public:
	ShaderProgram(std::initializer_list<Shader> shaders) :
		handle(glCreateProgram())
	{
		for (const auto& shader : shaders)
			glAttachShader(handle, shader.handle);

		glLinkProgram(handle);

		check_for_errors();
	}

	void use() const
	{
		glUseProgram(handle);
	}

	VertexAttribute get_attrib_location(const char* const name)
	{
		int location = glGetAttribLocation(handle, name);
		if (location == -1)
			throw std::runtime_error("the named attribute variable is not an active attribute in the specified program object or the name starts with the reserved prefix \"gl_\"");
		return VertexAttribute{ static_cast<unsigned int>(location) };
	}

	~ShaderProgram()
	{
		glDeleteProgram(handle);
	}
};