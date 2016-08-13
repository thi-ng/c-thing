#include <stdio.h>

#include "math/math.h"

ct_export float ct_parse_float(char *src, float err) {
  float x;
  return sscanf(src, "%f", &x) ? x : err;
}

ct_export int ct_parse_int(char *src, int err) {
  int x;
  return sscanf(src, "%d", &x) ? x : err;
}

ct_export uint32_t ct_parse_hex32(char *src, uint32_t err) {
  uint32_t x;
  return sscanf(src, "%x", &x) ? x : err;
}
