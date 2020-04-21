#pragma once

#include "Vendor.h"
#include "Viewer.h"

struct Camera
{
	Transform transform;
	std::unique_ptr<IViewer> viewer;
};