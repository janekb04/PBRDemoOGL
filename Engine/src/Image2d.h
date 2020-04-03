#pragma once

#include "Vendor.h"

class Image2d
{
private:
	unsigned char* m_data;
	GLenum m_internal_format;
	int m_width, m_height, m_channels;
private:
	Image2d(unsigned char* data, GLenum internal_format, int width, int height, int channels) :
		m_data{data},
		m_internal_format{ internal_format },
		m_width{width},
		m_height{height},
		m_channels{channels}
	{
	}
public:
	Image2d() = delete;
	Image2d(const Image2d&) = delete;

	Image2d(Image2d&& other) noexcept :
		m_data{ other.m_data },
		m_internal_format{ other.m_internal_format },
		m_width{ other.m_width },
		m_height{ other.m_height },
		m_channels{ other.m_channels }
	{
		other.m_data = nullptr;
		other.m_channels = 0;
	}

	static Image2d from_file(const std::string& path, bool is_srgb)
	{
		int width, height, channels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		if (!data)
			throw std::runtime_error("failed to load image");

		if (is_srgb)
		{
			//TODO: don't do this. it looses a lot of precision. just use GL_SRGB internal_format
			for (int i = 0; i < width * height * channels; ++i)
			{
				data[i] = (255.0 * pow(data[i] / 255.0 , 2.2)) + 0.5;
			}
		}

		GLenum internal_formats[]
		{
			GL_R8,
			GL_RG8,
			GL_RGB8,
			GL_RGBA8,
		};

		return Image2d(data, internal_formats[channels - 1], width, height, channels);
	}

	unsigned char* data() const noexcept
	{
		return m_data;
	}

	GLenum internal_format() const noexcept
	{
		return m_internal_format;
	}

	int width() const noexcept
	{
		return m_width;
	}

	int height() const noexcept
	{
		return m_height;
	}

	int channels() const noexcept
	{
		return m_channels;
	}

	~Image2d()
	{
		if (m_data)
			stbi_image_free(m_data);
	}
};