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

CT_Triangle2f *ct_triangle2f_init(CT_Triangle2f *t, const CT_Vec2f *a,
                                  const CT_Vec2f *b, const CT_Vec2f *c);
CT_Triangle2f *ct_triangle2f_initpv(CT_Triangle2f *t, const CT_Vec2f *points);
CT_Triangle2f *ct_triangle2f_initpf(CT_Triangle2f *t, const float *coords);

float ct_triangle2f_area(const CT_Triangle2f *t);
float ct_triangle2f_circumference(const CT_Triangle2f *t);
float ct_triangle2f_normsign(const CT_Triangle2f *t);
size_t ct_triangle2f_is_clockwise(const CT_Triangle2f *t);
CT_Vec2f *ct_triangle2f_point_opposite_ptr(CT_Triangle2f *t, const CT_Vec2f *a,
                                           const CT_Vec2f *b);
CT_Vec2f *ct_triangle2f_point_opposite(CT_Triangle2f *t, const CT_Vec2f *a,
                                       const CT_Vec2f *b);

///// CT_Triangle3f

CT_Triangle3f *ct_triangle3f_init(CT_Triangle3f *t, const CT_Vec3f *a,
                                  const CT_Vec3f *b, const CT_Vec3f *c);
CT_Triangle3f *ct_triangle3f_initpv(CT_Triangle3f *t, const CT_Vec3f *points);
CT_Triangle3f *ct_triangle3f_initpf(CT_Triangle3f *t, const float *coords);

float ct_triangle3f_area(CT_Triangle3f *t);
float ct_triangle3f_circumference(CT_Triangle3f *t);
CT_Vec3f *ct_triangle3f_normal(const CT_Triangle3f *t, CT_Vec3f *out,
                               size_t normalize);
float ct_triangle3f_normsign(const CT_Triangle3f *t);
size_t ct_triangle3f_is_clockwise(const CT_Triangle3f *t, const CT_Vec3f *n);
CT_Vec3f *ct_triangle3f_point_opposite_ptr(CT_Triangle3f *t, const CT_Vec3f *a,
                                           const CT_Vec3f *b);
CT_Vec3f *ct_triangle3f_point_opposite(CT_Triangle3f *t, const CT_Vec3f *a,
                                       const CT_Vec3f *b);

CT_END_DECLS
