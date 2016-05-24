#pragma once

#include "config.h"
#include "vec.h"

CT_BEGIN_DECLS

typedef union {
  struct {
    CT_Vec2f a, b, c;
  };
  float buf[3 * sizeof(CT_Vec2f) / sizeof(float)];
} CT_Triangle2f;

typedef union {
  struct {
    CT_Vec3f a, b, c;
  };
  float buf[3 * sizeof(CT_Vec3f) / sizeof(float)];
} CT_Triangle3f;

CT_EXPORT CT_Triangle2f *ct_triangle2f_init(CT_Triangle2f *t, CT_Vec2f *a,
                                            CT_Vec2f *b, CT_Vec2f *c);
CT_EXPORT CT_Triangle2f *ct_triangle2f_initpv(CT_Triangle2f *t,
                                              CT_Vec2f *points);
CT_EXPORT CT_Triangle2f *ct_triangle2f_initpf(CT_Triangle2f *t, float *coords);

CT_EXPORT float ct_triangle2f_area(CT_Triangle2f *t);
CT_EXPORT float ct_triangle2f_circumference(CT_Triangle2f *t);

CT_EXPORT CT_Triangle3f *ct_triangle3f_init(CT_Triangle3f *t, CT_Vec3f *a,
                                            CT_Vec3f *b, CT_Vec3f *c);
CT_EXPORT CT_Triangle3f *ct_triangle3f_initpv(CT_Triangle3f *t,
                                              CT_Vec3f *points);
CT_EXPORT CT_Triangle3f *ct_triangle3f_initpf(CT_Triangle3f *t, float *coords);

CT_EXPORT float ct_triangle3f_area(CT_Triangle3f *t);
CT_EXPORT float ct_triangle3f_circumference(CT_Triangle3f *t);

CT_END_DECLS
