#pragma once

#include <math.h>

#define EPS 1e-6f

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static inline uint8_t ct_deltaeqf(float a, float b, float eps) {
    return fabs(a - b) <= eps;
}

static inline float ct_mixf(float a, float b, float t) {
    return a + (b - a) * t;
}
