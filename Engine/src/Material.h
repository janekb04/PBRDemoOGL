#pragma once

#include "TextureHandle.h"

struct Material
{
	TextureHandle base_idx;
	TextureHandle gloss_idx;
	TextureHandle normal_idx;
};