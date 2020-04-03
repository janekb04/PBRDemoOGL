#pragma once

#include "Vendor.h"
#include "Image2d.h"

class Image2dArray
{
private:
	std::unique_ptr<unsigned char> m_data;
	GLenum m_internal_format;
	int m_width, m_height, m_depth, m_channels;
private:
	Image2dArray(std::unique_ptr<unsigned char>&& data, GLenum internal_format, int width, int height, int depth, int channels) :
		m_data{ std::move(data) },
		m_internal_format{ internal_format },
		m_width{ width },
		m_height{ height },
		m_depth{ depth },
		m_channels{ channels }
	{
	}
public:
	Image2dArray() = delete;
	Image2dArray(const Image2dArray&) = delete;

	template <typename It>
	static Image2dArray from_images(It begin, It end)
	{
		int width = begin->width();
		int height = begin->height();
		int depth = std::distance(begin, end);
		int channels = begin->channels();
		GLenum internal_format = begin->internal_format();

		std::unique_ptr<unsigned char> data{ new unsigned char[width * height * depth * channels] };

		for (int i = 0; begin != end; ++begin, ++i)
		{
			const Image2d& image = *begin;

			assert(image.width() == width);
			assert(image.height() == height);
			assert(image.internal_format() == internal_format);

			std::copy(
				image.data(),
				image.data() + width * height * channels,
				data.get() + width * height * channels * i
			);
		}

		return Image2dArray(std::move(data), internal_format, width, height, depth, channels);
	}

	void* data() const noexcept
	{
		return m_data.get();
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

	int depth() const noexcept
	{
		return m_depth;
	}

	int channels() const noexcept
	{
		return m_channels;
	}
};