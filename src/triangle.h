#pragma once

#include "vec.h"

typedef union {
  struct {
    CT_Vec2f a, b, c;
  };
  float buf[6];
} CT_Triangle2f;

CT_EXPORT CT_Triangle2f *ct_triangle2f_init(CT_Triangle2f *t, CT_Vec2f *a,
                                            CT_Vec2f *b, CT_Vec2f *c);
CT_EXPORT CT_Triangle2f *ct_triangle2f_initp(CT_Triangle2f *t,
                                             CT_Vec2f *points);
CT_EXPORT CT_Triangle2f *ct_triangle2f_initxy(CT_Triangle2f *t, float x1,
                                              float y1, float x2, float y2,
                                              float x3, float y3);
