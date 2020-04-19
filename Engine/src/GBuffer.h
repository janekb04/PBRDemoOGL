#pragma once

#include "Vendor.h"
#include "Framebuffer.h"
#include "Texture2d.h"

// Format
//  Attachment        x        y        z         w  
//	         0    pos.x    pos.y    pos.z        ao
//	         1 normal.x normal.y normal.z  metallic
//	         2 albedo.r albedo.g albedo.b roughness
class GBuffer
{
private:
	const unsigned width, height;
	Framebuffer fbo;
	Texture2d attachment0;
	Texture2d attachment1;
	Texture2d attachment2;
public:
	GBuffer(unsigned _width, unsigned _height) :
		width{_width},
		height{_height}
	{
		attachment0.storage(1, GL_RGBA16F, width, height);
		attachment1.storage(1, GL_RGBA16F, width, height);
		attachment2.storage(1, GL_RGBA16F, width, height);

		fbo.texture(GL_COLOR_ATTACHMENT0, attachment0, 0);
		fbo.texture(GL_COLOR_ATTACHMENT1, attachment1, 0);
		fbo.texture(GL_COLOR_ATTACHMENT2, attachment2, 0);

		GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		fbo.draw_buffers(3, attachments);

		if (fbo.check_status() != GL_FRAMEBUFFER_COMPLETE)
			throw std::runtime_error("framebuffer is not complete");
	}

	void bind() const
	{
		fbo.bind();
	}
};