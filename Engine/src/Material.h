#pragma once

#include "TextureHandle.h"

struct Material
{
	TextureHandle albedo_idx;
	TextureHandle normal_idx;
	TextureHandle metalic_idx;
	TextureHandle roughness_idx;
	TextureHandle ao_idx;
};