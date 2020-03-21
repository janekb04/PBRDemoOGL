#pragma once

#include "Vendor.h"

class Texture2dArray
{
private:
	GLuint handle;
private:
	static GLuint create_texture()
	{
		GLuint handle;
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &handle);
		return handle;
	}
	Texture2dArray(GLuint handle) :
		handle(handle)
	{
	}
public:
	Texture2dArray(const Texture2dArray&) = delete;
	static Texture2dArray from_files(const char* const*  paths, int count)
	{
		std::vector<GLubyte> data;
		int width, height, channels;

		assert(count > 0);

		for (int i = 0; i < count; ++i)
		{
			int _width, _height, _channels;
			unsigned char* _data = stbi_load(paths[i], &_width, &_height, &_channels, 0);

			if (!_data)
				throw std::runtime_error("failed to load texture " + std::to_string(i));
			if (i == 0)
			{
				width = _width;
				height = _height;
				channels = _channels;
			}
			else if (_width != width || _height != height)
			{
				stbi_image_free(_data);
				throw std::runtime_error("texture dimensions don't match: " + std::to_string(i));
			}
			else if (_channels != channels)
			{
				stbi_image_free(_data);
				throw std::runtime_error("texture channel count doesn't match: " + std::to_string(i));
			}
			
			int _size = _width * _height * _channels;
			data.insert(data.end(), _data, _data + _size);
			stbi_image_free(_data);
		}
		
		GLuint handle = create_texture();

		GLenum internal_formats[]
		{
			GL_R8,
			GL_RG8,
			GL_RGB8,
			GL_RGBA8,
		};
		glTextureStorage3D(handle, 1, internal_formats[channels - 1], width, height, count);
		GLenum formats[]
		{
			GL_RED,
			GL_RG,
			GL_RGB,
			GL_RGBA,
		};
		glTextureSubImage3D(handle, 0, 0, 0, 0, width, height, count, formats[channels - 1], GL_UNSIGNED_BYTE, data.data());

		return Texture2dArray(handle);
	}

	void bind() const
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, handle);
	}

	~Texture2dArray()
	{
		glDeleteTextures(1, &handle);
	}
};