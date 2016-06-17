#pragma once

#include <xmmintrin.h>
#include "math/vec.h"
#include "mem/mem.h"

#define VEC3OP_SSE(type, ptype, name, op)                                      \
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
    v->mmval op## = _mm_set1_ps(n);                                            \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##n(const type *v, ptype n, type *out) {       \
    out->mmval = v->mmval op _mm_set1_ps(n);                                   \
    return out;                                                                \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##xyz_imm(type *v, ptype x, ptype y,           \
                                          ptype z) {                           \
    __m128 b      = _mm_set_ps(0, z, y, x);                                    \
    v->mmval op## = b;                                                         \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##xyz(const type *v, ptype x, ptype y,         \
                                      ptype z, type *out) {                    \
    __m128 b   = _mm_set_ps(0, z, y, x);                                       \
    out->mmval = v->mmval op b;                                                \
    return out;                                                                \
  }

CT_BEGIN_DECLS

typedef union {
  struct {
    float x, y, z;
  };
  float buf[3];
  __m128 mmval;
} CT_Vec3f CT_ALIGN(16);

VEC3OP_SSE(CT_Vec3f, float, ct_add3f, +)
VEC3OP_SSE(CT_Vec3f, float, ct_sub3f, -)
VEC3OP_SSE(CT_Vec3f, float, ct_mul3f, *)
VEC3OP_SSE(CT_Vec3f, float, ct_div3f, /)

CT_EXPORT ct_inline float ct_dot3fv(const CT_Vec3f *a, const CT_Vec3f *b) {
  __m128 d = a->mmval * b->mmval;
  return d[0] + d[1] + d[2];
}

CT_EXPORT ct_inline CT_Vec3f *ct_cross3fv_imm(CT_Vec3f *a, const CT_Vec3f *b) {
  __m128 v = _mm_sub_ps(
      _mm_mul_ps(a->mmval,
                 _mm_shuffle_ps(b->mmval, b->mmval, _MM_SHUFFLE(3, 0, 2, 1))),
      _mm_mul_ps(b->mmval,
                 _mm_shuffle_ps(a->mmval, a->mmval, _MM_SHUFFLE(3, 0, 2, 1))));
  a->mmval = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 0, 2, 1));
  return a;
}

CT_EXPORT ct_inline CT_Vec3f *ct_cross3fv(const CT_Vec3f *a, const CT_Vec3f *b,
                                          CT_Vec3f *out) {
  out->mmval = a->mmval;
  return ct_cross3fv_imm(out, b);
}

CT_EXPORT ct_inline float ct_distsq3fv(const CT_Vec3f *a, const CT_Vec3f *b) {
  __m128 d = a->mmval - b->mmval;
  d *= d;
  return d[0] + d[1] + d[2];
}

CT_EXPORT ct_inline CT_Vec3f *ct_madd3fv_imm(CT_Vec3f *a, const CT_Vec3f *b,
                                             const CT_Vec3f *c) {
  a->mmval = a->mmval * b->mmval + c->mmval;
  return a;
}

CT_EXPORT ct_inline float ct_magsq3f(const CT_Vec3f *v) {
  __m128 d = v->mmval * v->mmval;
  return d[0] + d[1] + d[2];
}

CT_EXPORT ct_inline CT_Vec3f *ct_max3fv_imm(CT_Vec3f *a, CT_Vec3f *b) {
  a->mmval = _mm_max_ps(a->mmval, b->mmval);
  return a;
}

CT_EXPORT ct_inline CT_Vec3f *ct_min3fv_imm(CT_Vec3f *a, CT_Vec3f *b) {
  a->mmval = _mm_min_ps(a->mmval, b->mmval);
  return a;
}

CT_EXPORT ct_inline CT_Vec3f *ct_mix3fv_imm(CT_Vec3f *a, const CT_Vec3f *b,
                                            float t) {
  a->mmval += (b->mmval - a->mmval) * _mm_load_ps1(&t);
  return a;
}

CT_EXPORT ct_inline CT_Vec3f *ct_normalize3f_imm(CT_Vec3f *v, float len) {
  float m = sqrt(ct_magsq3f(v));
  if (m > 0.0) {
    ct_mul3fn_imm(v, len / m);
  }
  return v;
}

CT_EXPORT ct_inline CT_Vec3f *ct_set3fv(CT_Vec3f *a, const CT_Vec3f *b) {
  a->mmval = b->mmval;
  return a;
}

CT_EXPORT ct_inline CT_Vec3f *ct_set3fn(CT_Vec3f *v, float n) {
  v->mmval = _mm_load_ps1(&n);
  return v;
}

CT_EXPORT ct_inline CT_Vec3f *ct_set3fp(CT_Vec3f *v, const float *p) {
  v->mmval = _mm_load_ps(p);
  return v;
}

CT_EXPORT ct_inline CT_Vec3f *ct_set3fpua(CT_Vec3f *v, const float *p) {
  v->mmval = _mm_loadu_ps(p);
  return v;
}

CT_END_DECLS
