#pragma once

#include <xmmintrin.h>
#include "vec.h"

#define VEC3OP_SSE(type, ptype, name, op)                                 \
  CT_EXPORT ct_inline type *name##v_imm(type *a, type *b) {               \
    a->mmval op## = b->mmval;                                             \
    return a;                                                             \
  }                                                                       \
                                                                          \
  CT_EXPORT ct_inline type *name##v(type *a, type *b, type *out) {        \
    out->mmval = a->mmval op b->mmval;                                    \
    return out;                                                           \
  }                                                                       \
                                                                          \
  CT_EXPORT ct_inline type *name##n_imm(type *v, ptype n) {               \
    __m128 b = {n, n, n, 0.f};                                            \
    v->mmval op## = b;                                                    \
    return v;                                                             \
  }                                                                       \
                                                                          \
  CT_EXPORT ct_inline type *name##n(type *v, ptype n, type *out) {        \
    __m128 b = {n, n, n, 0.f};                                            \
    out->mmval = v->mmval op b;                                           \
    return out;                                                           \
  }                                                                       \
                                                                          \
  CT_EXPORT ct_inline type *name##xyz_imm(type *v, ptype x, ptype y,      \
                                          ptype z) {                      \
    __m128 b = {x, y, z, 0.f};                                            \
    v->mmval op## = b;                                                    \
    return v;                                                             \
  }                                                                       \
                                                                          \
  CT_EXPORT ct_inline type *name##xyz(type *v, ptype x, ptype y, ptype z, \
                                      type *out) {                        \
    __m128 b = {x, y, z, 0.f};                                            \
    out->mmval = v->mmval op b;                                           \
    return out;                                                           \
  }

typedef union {
  struct {
    float x, y, z;
  };
  float buf[3];
  __m128 mmval;
} CT_Vec3f;

VEC3OP_SSE(CT_Vec3f, float, ct_add3f, +)
VEC3OP_SSE(CT_Vec3f, float, ct_sub3f, -)
VEC3OP_SSE(CT_Vec3f, float, ct_mul3f, *)
VEC3OP_SSE(CT_Vec3f, float, ct_div3f, /)

CT_EXPORT ct_inline float ct_dot3fv(CT_Vec3f *a, CT_Vec3f *b) {
  __m128 d = a->mmval * b->mmval;
  return d[0] + d[1] + d[2];
}

CT_EXPORT ct_inline CT_Vec3f *ct_cross3fv_imm(CT_Vec3f *a, CT_Vec3f *b) {
  __m128 v = _mm_sub_ps(
      _mm_mul_ps(a->mmval,
                 _mm_shuffle_ps(b->mmval, b->mmval, _MM_SHUFFLE(3, 0, 2, 1))),
      _mm_mul_ps(b->mmval,
                 _mm_shuffle_ps(a->mmval, a->mmval, _MM_SHUFFLE(3, 0, 2, 1))));
  a->mmval = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 0, 2, 1));
  return a;
}

CT_EXPORT ct_inline float ct_distsq3fv(CT_Vec3f *a, CT_Vec3f *b) {
  __m128 d = a->mmval - b->mmval;
  d *= d;
  return d[0] + d[1] + d[2];
}

CT_EXPORT ct_inline CT_Vec3f *ct_madd3fv_imm(CT_Vec3f *a, CT_Vec3f *b,
                                             CT_Vec3f *c) {
  a->mmval = a->mmval * b->mmval + c->mmval;
  return a;
}

CT_EXPORT ct_inline float ct_magsq3f(CT_Vec3f *v) {
  __m128 d = v->mmval * v->mmval;
  return d[0] + d[1] + d[2];
}

CT_EXPORT ct_inline CT_Vec3f *ct_mix3fv_imm(CT_Vec3f *a, CT_Vec3f *b, float t) {
  __m128 mt = {t, t, t, 0.f};
  a->mmval += (b->mmval - a->mmval) * mt;
  return a;
}

CT_EXPORT ct_inline CT_Vec3f *ct_normalize3f_imm(CT_Vec3f *v, float len) {
  float m = sqrt(ct_magsq3f(v));
  if (m > 0.0) {
    ct_mul3fn_imm(v, len / m);
  }
  return v;
}

CT_EXPORT ct_inline CT_Vec3f *ct_set3fv(CT_Vec3f *a, CT_Vec3f *b) {
  a->mmval = b->mmval;
  return a;
}
