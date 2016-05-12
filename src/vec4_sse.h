#pragma once

#include <xmmintrin.h>
#include "vec.h"

typedef union {
    struct {
        float x, y, z, w;
    };
    float buf[4];
    __m128 mmval;
} CT_Vec4f;
