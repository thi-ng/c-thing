#pragma once

#include <xmmintrin.h>
#include "vec.h"

#define VEC4OP_SSE(type, ptype, name, op)                                      \
  CT_EXPORT ct_inline type *name##v_imm(type *a, type *b) {                    \
    a->mmval op## = b->mmval;                                                  \
    return a;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##v(type *a, type *b, CT_MPool *mpool) {       \
    type *v = ALLOCATE_TYPE(mpool, type);                                      \
    v->mmval = a->mmval op b->mmval;                                           \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##n_imm(type *v, ptype n) {                    \
    __m128 b = {n, n, n, n};                                                   \
    v->mmval op## = b;                                                         \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##n(type *a, ptype n, CT_MPool *mpool) {       \
    type *v = ALLOCATE_TYPE(mpool, type);                                      \
    __m128 b = {n, n, n, n};                                                   \
    v->mmval = a->mmval op b;                                                  \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##xyzw_imm(type *v, ptype x, ptype y, ptype z, \
                                           ptype w) {                          \
    __m128 b = {x, y, z, w};                                                   \
    v->mmval op## = b;                                                         \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##xyzw(type *a, ptype x, ptype y, ptype z,     \
                                       ptype w, CT_MPool *mpool) {             \
    type *v = ALLOCATE_TYPE(mpool, type);                                      \
    __m128 b = {x, y, z, w};                                                   \
    v->mmval = a->mmval op b;                                                  \
    return v;                                                                  \
  }

typedef union {
  struct {
    float x, y, z, w;
  };
  float buf[4];
  __m128 mmval;
} CT_Vec4f;

VEC4OP_SSE(CT_Vec4f, float, ct_add4f, +)
VEC4OP_SSE(CT_Vec4f, float, ct_sub4f, -)
VEC4OP_SSE(CT_Vec4f, float, ct_mul4f, *)
VEC4OP_SSE(CT_Vec4f, float, ct_div4f, /)

CT_EXPORT ct_inline float ct_dot4fv(CT_Vec4f *a, CT_Vec4f *b) {
  __m128 d = a->mmval * b->mmval;
  return d[0] + d[1] + d[2] + d[3];
}

CT_EXPORT ct_inline float ct_distsq4fv(CT_Vec4f *a, CT_Vec4f *b) {
  __m128 d = a->mmval - b->mmval;
  d *= d;
  return d[0] + d[1] + d[2] + d[3];
}

CT_EXPORT ct_inline CT_Vec4f *ct_madd4fv_imm(CT_Vec4f *a, CT_Vec4f *b,
                                             CT_Vec4f *c) {
  a->mmval = a->mmval * b->mmval + c->mmval;
  return a;
}

CT_EXPORT ct_inline float ct_magsq4f(CT_Vec4f *v) {
  __m128 d = v->mmval * v->mmval;
  return d[0] + d[1] + d[2] + d[3];
}

CT_EXPORT ct_inline CT_Vec4f *ct_mix4fv_imm(CT_Vec4f *a, CT_Vec4f *b, float t) {
  __m128 mt = {t, t, t, t};
  a->mmval += (b->mmval - a->mmval) * mt;
  return a;
}

CT_EXPORT ct_inline CT_Vec4f *ct_normalize4f_imm(CT_Vec4f *v, float len) {
  float m = sqrt(ct_magsq4f(v));
  if (m > 0.0) {
    ct_mul4fn_imm(v, len / m);
  }
  return v;
}

CT_EXPORT ct_inline CT_Vec4f *ct_set4fv(CT_Vec4f *a, CT_Vec4f *b) {
  a->mmval = b->mmval;
  return a;
}
