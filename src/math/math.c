#include <stdio.h>

#include "math/math.h"

CT_EXPORT uint32_t ct_ceil_pow2(uint32_t x) {
  if (x) {
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
  }
  return 1;
}

CT_EXPORT uint32_t ct_floor_pow2(uint32_t x) {
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  return x - (x >> 1);
}

CT_EXPORT float ct_parse_float(char *src, float err) {
  float x;
  return sscanf(src, "%f", &x) ? x : err;
}

CT_EXPORT uint32_t ct_parse_hex32(char *src, uint32_t err) {
  uint32_t x;
  return sscanf(src, "%x", &x) ? x : err;
}
