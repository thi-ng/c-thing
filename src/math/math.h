#pragma once

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "cthing.h"

#ifndef EPS
#define EPS 1e-6f
#endif
#define LN2 0.69314718055994530942f
#define SQRT2 1.414213562373095f

#define PI 3.1415926535897932384626433832795029f
#define TAU (2.0f * PI)
#define HALF_PI (0.5f * PI)
#define INV_TAU (1.0f / TAU)
#define INV_PI (1.0f / PI)
#define INV_HALF_PI (1.0f / HALF_PI)
#define DEGREES (180.0 / PI)
#define RADIANS (PI / 180.0)

#define INV_RAND_MAX (float)(1.0 / RAND_MAX)
#define INV_RAND_MAX2 (float)(2.0 / RAND_MAX)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN_LET(type, out, a, b)           \
  {                                        \
    type ta = (a);                         \
    type tb = (b);                         \
    out     = ((ta) < (tb) ? (ta) : (tb)); \
  }
#define MAX_LET(type, out, a, b)           \
  {                                        \
    type ta = (a);                         \
    type tb = (b);                         \
    out     = ((ta) > (tb) ? (ta) : (tb)); \
  }
#define CLAMP(a, b, c) MIN(MAX(a, b), c)
#define IN_RANGE(x, a, b) ((x) >= (a) && (x) <= (b))
#define SIGNUM(x, eps) (int)((x) < 0 ? -1 : ((x) > 0) ? 1 : 0)
#define COMPARE(a, b) (int)((a) < (b) ? -1 : ((a) > (b) ? 1 : 0))

CT_BEGIN_DECLS

uint32_t ct_ceil_pow2(uint32_t x);
uint32_t ct_floor_pow2(uint32_t x);
float ct_parse_float(char *src, float err);
int ct_parse_int(char *src, int err);
uint32_t ct_parse_hex32(char *src, uint32_t err);

ct_export ct_inline float ct_clampf(const float a,
                                    const float b,
                                    const float c) {
  return CLAMP(a, b, c);
}

ct_export ct_inline size_t ct_clz8(uint8_t x) {
#if defined(__GCC__) || defined(__clang__) || defined(__EMSCRIPTEN__)
  return x == 0 ? 8 : __builtin_clz(x) - 24;
#else
  return 0;  // FIXME
#endif
}

ct_export ct_inline int ct_deltaeqf(const float a,
                                    const float b,
                                    const float eps) {
  return fabs(a - b) <= eps;
}

ct_inline float ct_maddf(const float a, const float b, const float c) {
  return a * b + c;
}

ct_inline float ct_mixf(const float a, const float b, const float t) {
  return a + (b - a) * t;
}

ct_inline size_t ct_ceil_multiple_pow2(const size_t x, const size_t y) {
  return (x + (y - 1)) & ~(y - 1);
}

ct_inline float ct_rand_norm() {
  return rand() * INV_RAND_MAX2 - 1.0f;
}

ct_inline float ct_rand_normpos() {
  return rand() * INV_RAND_MAX;
}

ct_inline float ct_rand_minmax(const float min, const float max) {
  return ct_rand_normpos() * (max - min) + min;
}

ct_inline int ct_signumf(const float x, const float eps) {
  if (ct_deltaeqf(x, 0.f, eps)) {
    return 0;
  }
  return (x < 0.f ? -1 : 1);
}

ct_inline float ct_stepf(const float x,
                         const float edge,
                         const float y1,
                         const float y2) {
  return (x < edge ? y1 : y2);
}

ct_inline float ct_wrapf(float x, float domain) {
  return ((x < 0.f) ? (domain + x) : (x >= domain ? (x - domain) : x));
}

ct_inline uint32_t ct_rotl32(uint32_t x, uint8_t r) {
  return (x << r) | (x >> (32 - r));
}

ct_inline uint64_t ct_rotl64(uint64_t x, uint8_t r) {
  return (x << r) | (x >> (64 - r));
}

CT_END_DECLS
