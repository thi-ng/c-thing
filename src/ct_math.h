#pragma once

#include <math.h>
#include "config.h"

#define EPS 1e-6f
#define LN2 0.69314718055994530942f
#define SQRT2 1.414213562373095f

#define PI 3.1415926535897932384626433832795029f
#define TAU (2.0f * PI)
#define HALF_PI (0.5f * PI)
#define INV_TAU (1.0f / TAU)
#define INV_PI (1.0f / PI)
#define INV_HALF_PI (1.0f / HALF_PI)

#define INV_RAND_MAX (float)(1.0 / UINT32_MAX)
#define INV_RAND_MAX2 (float)(2.0 / UINT32_MAX)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SIGNUM(x, eps) (int8_t)((x) < 0 ? -1 : ((x) > 0) ? 1 : 0)

CT_EXPORT ct_inline uint8_t ct_deltaeqf(const float a, const float b,
                                        const float eps) {
  return fabs(a - b) <= eps;
}

ct_inline float ct_maddf(const float a, const float b, const float c) {
  return a * b + c;
}

ct_inline float ct_mixf(const float a, const float b, const float t) {
  return a + (b - a) * t;
}

ct_inline int8_t ct_signumf(const float x, const float eps) {
  if (ct_deltaeqf(x, 0.f, eps)) {
    return 0;
  }
  return (x < 0.f ? -1 : 1);
}

ct_inline float ct_stepf(const float x, const float edge, const float y1,
                         const float y2) {
  return (x < edge ? y1 : y2);
}
