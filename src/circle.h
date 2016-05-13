#pragma once

#include "mpool.h"
//#include "protocols.h"
#include "vec.h"

typedef union {
  struct {
  CT_Vec2f p;
  float r;
  };
  float buf[3];
} CT_Circle2f;

CT_Circle2f *ct_circle2fxy(float x, float y, float r, CT_MPool *mpool);
CT_Circle2f *ct_circle2fv(CT_Vec2f *p, float r, CT_MPool *mpool);
CT_Circle2f *ct_circle2fr(float r, CT_MPool *mpool);

float ct_circle2f_area(CT_Circle2f *c);
float ct_circle2f_circumference(CT_Circle2f *c);
int8_t ct_circle2f_classify_point2f(CT_Circle2f *c, CT_Vec2f *p);

CT_Vec2f *ct_circle2f_vertices(CT_Circle2f *c, CT_Vec2f *verts, uint32_t n);
