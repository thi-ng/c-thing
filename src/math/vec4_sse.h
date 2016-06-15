#pragma once

#include <xmmintrin.h>
#include "math/vec.h"

#define VEC4OP_SSE(type, ptype, name, op)                                      \
  CT_EXPORT ct_inline type *name##v_imm(type *a, const type *b) {              \
    a->mmval op## = b->mmval;                                                  \
    return a;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##v(const type *a, const type *b, type *out) { \
    out->mmval = a->mmval op b->mmval;                                         \
    return out;                                                                \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##n_imm(type *v, ptype n) {                    \
    __m128 b      = {n, n, n, n};                                              \
    v->mmval op## = b;                                                         \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##n(const type *v, ptype n, type *out) {       \
    __m128 b   = {n, n, n, n};                                                 \
    out->mmval = v->mmval op b;                                                \
    return out;                                                                \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##xyzw_imm(type *v, ptype x, ptype y, ptype z, \
                                           ptype w) {                          \
    __m128 b      = {x, y, z, w};                                              \
    v->mmval op## = b;                                                         \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##xyzw(const type *v, ptype x, ptype y,        \
                                       ptype z, ptype w, type *out) {          \
    __m128 b   = {x, y, z, w};                                                 \
    out->mmval = v->mmval op b;                                                \
    return out;                                                                \
  }

CT_BEGIN_DECLS

__attribute__((aligned(16))) typedef union {
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

CT_EXPORT ct_inline float ct_dot4fv(const CT_Vec4f *a, const CT_Vec4f *b) {
  __m128 d = a->mmval * b->mmval;
  return d[0] + d[1] + d[2] + d[3];
}

CT_EXPORT ct_inline float ct_distsq4fv(const CT_Vec4f *a, const CT_Vec4f *b) {
  __m128 d = a->mmval - b->mmval;
  d *= d;
  return d[0] + d[1] + d[2] + d[3];
}

CT_EXPORT ct_inline CT_Vec4f *ct_madd4fv_imm(CT_Vec4f *a, const CT_Vec4f *b,
                                             const CT_Vec4f *c) {
  a->mmval = a->mmval * b->mmval + c->mmval;
  return a;
}

CT_EXPORT ct_inline float ct_magsq4f(const CT_Vec4f *v) {
  __m128 d = v->mmval * v->mmval;
  return d[0] + d[1] + d[2] + d[3];
}

CT_EXPORT ct_inline CT_Vec4f *ct_mix4fv_imm(CT_Vec4f *a, const CT_Vec4f *b,
                                            float t) {
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

CT_EXPORT ct_inline CT_Vec4f *ct_set4fv(CT_Vec4f *a, const CT_Vec4f *b) {
  a->mmval = b->mmval;
  return a;
}

CT_EXPORT ct_inline CT_Vec4f *ct_set4fp(CT_Vec4f *v, const float *p) {
  v->mmval = _mm_load_ps(p);
  return v;
}

CT_END_DECLS
