#pragma once

#include "../Vendor.h"
#include "Vertex.h"
#include "Index.h"

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<Index> indices;
};