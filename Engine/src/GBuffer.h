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
	GBuffer(unsigned _width, unsigned _height)
	{

	}
};