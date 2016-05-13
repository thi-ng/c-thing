#pragma once

//#include "protocols.h"
#include "vec.h"

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
int8_t ct_circle2f_classify_point(CT_Circle2f *c, CT_Vec2f *p);

CT_Vec2f *ct_circle2f_vertices(CT_Circle2f *c, CT_Vec2f *verts, uint32_t n);
