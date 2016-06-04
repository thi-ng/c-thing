#pragma once

#include "config.h"
//#include "geom/protocols.h"
#include "geom/triangle.h"
#include "math/vec.h"

CT_BEGIN_DECLS

typedef union {
  struct {
    CT_Vec2f pos;
    float r;
  };
  float buf[3];
} CT_Circle2f;

CT_Circle2f *ct_circle2f_init(CT_Circle2f *c, float x, float y, float r);
CT_Circle2f *ct_circle2f_initvr(CT_Circle2f *c, CT_Vec2f *p, float r);
CT_Circle2f *ct_circle2f_initr(CT_Circle2f *c, float r);

float ct_circle2f_area(CT_Circle2f *c);
float ct_circle2f_circumference(CT_Circle2f *c);
int ct_circle2f_classify_point(CT_Circle2f *c, CT_Vec2f *p);

CT_Vec2f *ct_circle2f_vertices(CT_Circle2f *c, CT_Vec2f *verts, size_t n);
CT_Triangle2f *ct_circle2f_tessellate(CT_Circle2f *c, CT_Triangle2f *tris,
                                      size_t n);

CT_END_DECLS
