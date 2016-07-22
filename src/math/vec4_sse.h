#pragma once

#include <xmmintrin.h>
#ifdef CT_FEATURE_SSE3
#include <pmmintrin.h>
#endif
#ifdef CT_FEATURE_SSE4
#include <smmintrin.h>
#endif
#include "math/vec.h"
#include "mem/mem.h"

#define VEC4OP_SSE(type, ptype, name, op)                                      \
  ct_export ct_inline type *name##v_imm(type *a, const type *b) {              \
    a->mmval op## = b->mmval;                                                  \
    return a;                                                                  \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##v(const type *a, const type *b, type *out) { \
    out->mmval = a->mmval op b->mmval;                                         \
    return out;                                                                \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##n_imm(type *v, ptype n) {                    \
    v->mmval op## = _mm_set1_ps(n);                                            \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##n(const type *v, ptype n, type *out) {       \
    out->mmval = v->mmval op _mm_set1_ps(n);                                   \
    return out;                                                                \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##xyzw_imm(type *v, ptype x, ptype y, ptype z, \
                                           ptype w) {                          \
    __m128 b      = _mm_set_ps(w, z, y, x);                                    \
    v->mmval op## = b;                                                         \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##xyzw(const type *v, ptype x, ptype y,        \
                                       ptype z, ptype w, type *out) {          \
    __m128 b   = _mm_set_ps(w, z, y, x);                                       \
    out->mmval = v->mmval op b;                                                \
    return out;                                                                \
  }

CT_BEGIN_DECLS

typedef union {
  struct {
    float x, y, z, w;
  };
  CT_Vec2f xy;
  CT_Vec3f xyz;
  float buf[4];
  __m128 mmval;
} CT_Vec4f CT_ALIGN(16);

VEC4OP_SSE(CT_Vec4f, float, ct_add4f, +)
VEC4OP_SSE(CT_Vec4f, float, ct_sub4f, -)
VEC4OP_SSE(CT_Vec4f, float, ct_mul4f, *)
VEC4OP_SSE(CT_Vec4f, float, ct_div4f, /)

ct_export ct_inline float ct_dot4fv(const CT_Vec4f *a, const CT_Vec4f *b) {
#ifdef CT_FEATURE_SSE4
  return _mm_dp_ps(a->mmval, b->mmval, 0xf1)[0];
#else
  __m128 d = a->mmval * b->mmval;
#ifdef CT_FEATURE_SSE3
  d        = _mm_hadd_ps(d, d);
  return d[0] + d[1];
#else
  return d[0] + d[1] + d[2] + d[3];
#endif
#endif
}

ct_export ct_inline float ct_distsq4fv(const CT_Vec4f *a, const CT_Vec4f *b) {
  __m128 d = a->mmval - b->mmval;
  d *= d;
#ifdef CT_FEATURE_SSE3
  d = _mm_hadd_ps(d, d);
  return d[0] + d[1];
#else
  return d[0] + d[1] + d[2] + d[3];
#endif
}

ct_export ct_inline CT_Vec4f *ct_madd4fv_imm(CT_Vec4f *a,
                                             const CT_Vec4f *b,
                                             const CT_Vec4f *c) {
  a->mmval = a->mmval * b->mmval + c->mmval;
  return a;
}

ct_export ct_inline float ct_magsq4f(const CT_Vec4f *v) {
  return ct_dot4fv(v, v);
}

ct_export ct_inline CT_Vec4f *ct_mix4fv_imm(CT_Vec4f *a,
                                            const CT_Vec4f *b,
                                            float t) {
  a->mmval += (b->mmval - a->mmval) * _mm_set1_ps(t);
  return a;
}

ct_export ct_inline CT_Vec4f *ct_negate4f_imm(CT_Vec4f *v) {
  v->mmval = _mm_setzero_ps() - v->mmval;
  return v;
}

ct_export ct_inline CT_Vec4f *ct_normalize4f_imm(CT_Vec4f *v, float len) {
  float m = sqrt(ct_magsq4f(v));
  if (m > 0.0) {
    ct_mul4fn_imm(v, len / m);
  }
  return v;
}

ct_export ct_inline CT_Vec4f *ct_set4fv(CT_Vec4f *a, const CT_Vec4f *b) {
  a->mmval = b->mmval;
  return a;
}

ct_export ct_inline CT_Vec4f *ct_set4fn(CT_Vec4f *v, float n) {
  v->mmval = _mm_set1_ps(n);
  return v;
}

ct_export ct_inline CT_Vec4f *ct_set4fxyzw(CT_Vec4f *v,
                                           float x,
                                           float y,
                                           float z,
                                           float w) {
  v->mmval = _mm_set_ps(w, z, y, x);
  return v;
}

ct_export ct_inline CT_Vec4f *ct_set4fp(CT_Vec4f *v, const float *p) {
  v->mmval = _mm_load_ps(p);
  return v;
}

ct_export ct_inline CT_Vec4f *ct_set4fpua(CT_Vec4f *v, const float *p) {
  v->mmval = _mm_loadu_ps(p);
  return v;
}

CT_END_DECLS
