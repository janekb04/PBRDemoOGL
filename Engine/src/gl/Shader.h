#pragma once

#include "../Vendor.h"
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

class Uniform
{
private:
	GLuint m_index;
public:
	explicit Uniform(unsigned int location) :
		m_index{ location }
	{
	}

	unsigned int index() const
	{
		return m_index;
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

	void uniform(Uniform uniform, bool transpose, const glm::mat4& matrix) const
	{
		glProgramUniformMatrix4fv(handle, uniform.index(), 1, transpose, glm::value_ptr(matrix));
	}

	void uniform(Uniform uniform, const glm::vec3& vec) const
	{
		glProgramUniform3fv(handle, uniform.index(), 1, &vec.x);
	}

	void uniform(Uniform uniform, const glm::uvec2& vec) const
	{
		glProgramUniform2uiv(handle, uniform.index(), 1, &vec.x);
	}

	VertexAttribute get_attrib_location(const char* const name)
	{
		int location = glGetAttribLocation(handle, name);
		assert(location != -1);
		return VertexAttribute{ static_cast<unsigned int>(location) };
	}

	Uniform get_uniform_location(const char* const name)
	{
		int location = glGetUniformLocation(handle, name);
		assert(location != -1);
		return Uniform{ static_cast<unsigned int>(location) };
	}

	~ShaderProgram()
	{
		glDeleteProgram(handle);
	}
};