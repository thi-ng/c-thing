#pragma once

#include "mpool.h"
#include "vec.h"

typedef union {
  struct {
    CT_Vec2f a, b, c;
  };
  float buf[6];
} CT_Triangle2f;
