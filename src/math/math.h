#pragma once

#include <math.h>

#include "ct-head/math.h"

CT_BEGIN_DECLS

float ct_parse_float(char *src, float err);
int ct_parse_int(char *src, int err);
uint32_t ct_parse_hex32(char *src, uint32_t err);

CT_END_DECLS
