#pragma once

#include <math.h>
#include "config.h"

#define EPS 1e-6f

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

CT_EXPORT ct_inline uint8_t ct_deltaeqf(float a, float b, float eps) {
  return fabs(a - b) <= eps;
}

CT_EXPORT ct_inline float ct_mixf(float a, float b, float t) {
  return a + (b - a) * t;
}
