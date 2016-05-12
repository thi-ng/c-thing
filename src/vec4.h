#pragma once

#include "vec.h"

typedef union {
    struct {
        float x, y, z, w;
    };
    float buf[4];
} CT_Vec4f;
