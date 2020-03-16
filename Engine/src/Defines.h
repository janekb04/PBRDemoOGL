#pragma once

#include "Vendor.h"

struct glDrawElementsIndirectCommand {
    GLuint  count;
    GLuint  primCount;
    GLuint  firstIndex;
    GLuint  baseVertex;
    GLuint  baseInstance;
};