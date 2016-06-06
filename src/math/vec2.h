#pragma once

#include "math/vec.h"

#define VEC2OP(type, ptype, name, op)                                          \
  CT_EXPORT ct_inline type *name##v_imm(type *a, const type *b) {              \
    a->x op## = b->x;                                                          \
    a->y op## = b->y;                                                          \
    return a;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##v(const type *a, const type *b, type *out) { \
    out->x = a->x op b->x;                                                     \
    out->y = a->y op b->y;                                                     \
    return out;                                                                \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##n_imm(type *v, ptype n) {                    \
    v->x op## = n;                                                             \
    v->y op## = n;                                                             \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##n(const type *v, ptype n, type *out) {       \
    out->x = v->x op n;                                                        \
    out->y = v->y op n;                                                        \
    return out;                                                                \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##xy_imm(type *v, ptype x, ptype y) {          \
    v->x op## = x;                                                             \
    v->y op## = y;                                                             \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##xy(const type *v, ptype x, ptype y,          \
                                     type *out) {                              \
    out->x = v->x op x;                                                        \
    out->y = v->y op y;                                                        \
    return out;                                                                \
  }

CT_BEGIN_DECLS

typedef union {
  struct {
    float x, y;
  };
  float buf[2];
} CT_Vec2f;

VEC2OP(CT_Vec2f, float, ct_add2f, +)
VEC2OP(CT_Vec2f, float, ct_sub2f, -)
VEC2OP(CT_Vec2f, float, ct_mul2f, *)
VEC2OP(CT_Vec2f, float, ct_div2f, /)

CT_EXPORT CT_Vec2f *ct_vec2f(float x, float y, CT_MPool *mpool);
CT_EXPORT CT_Vec2f *ct_vec2fn(float n, CT_MPool *mpool);

CT_EXPORT ct_inline CT_Vec2f *ct_set2fv(CT_Vec2f *a, const CT_Vec2f *b) {
  a->x = b->x;
  a->y = b->y;
  return a;
}

CT_EXPORT ct_inline CT_Vec2f *ct_set2fxy(CT_Vec2f *v, float x, float y) {
  v->x = x;
  v->y = y;
  return v;
}

CT_EXPORT ct_inline CT_Vec2f *ct_set2fn(CT_Vec2f *v, float n) {
  v->x = n;
  v->y = n;
  return v;
}

CT_EXPORT ct_inline CT_Vec2f *ct_set2fp(CT_Vec2f *v, float *xy) {
  v->x = *xy++;
  v->y = *xy;
  return v;
}

CT_EXPORT ct_inline int ct_compare2fv(const CT_Vec2f *a, const CT_Vec2f *b) {
  int c = COMPARE(a->x, b->x);
  if (!c) {
    return COMPARE(a->y, b->y);
  }
  return c;
}

CT_EXPORT ct_inline size_t ct_deltaeq2fv(const CT_Vec2f *a, const CT_Vec2f *b,
                                         float eps) {
  return (ct_deltaeqf(a->x, b->x, eps) && ct_deltaeqf(a->y, b->y, eps));
}

CT_EXPORT ct_inline CT_Vec2f *ct_madd2fv_imm(CT_Vec2f *a, const CT_Vec2f *b,
                                             const CT_Vec2f *c) {
  a->x = a->x * b->x + c->x;
  a->y = a->y * b->y + c->y;
  return a;
}

CT_EXPORT ct_inline CT_Vec2f *ct_madd2fv(const CT_Vec2f *a, const CT_Vec2f *b,
                                         const CT_Vec2f *c, CT_Vec2f *out) {
  return ct_madd2fv_imm(ct_set2fv(out, a), b, c);
}

CT_EXPORT ct_inline float ct_dot2fv(const CT_Vec2f *a, const CT_Vec2f *b) {
  return a->x * b->x + a->y * b->y;
}

CT_EXPORT ct_inline float ct_distsq2fv(const CT_Vec2f *a, const CT_Vec2f *b) {
  float dx = a->x - b->x;
  float dy = a->y - b->y;
  return dx * dx + dy * dy;
}

CT_EXPORT ct_inline float ct_dist2fv(const CT_Vec2f *a, const CT_Vec2f *b) {
  return sqrtf(ct_distsq2fv(a, b));
}

CT_EXPORT ct_inline uint32_t ct_hash2f(const CT_Vec2f *v) {
  return ct_murmur3_32(v, 2 * sizeof(float));
}

CT_EXPORT ct_inline float ct_magsq2f(const CT_Vec2f *v) {
  return v->x * v->x + v->y * v->y;
}

CT_EXPORT ct_inline float ct_mag2f(const CT_Vec2f *v) {
  return sqrtf(v->x * v->x + v->y * v->y);
}

CT_EXPORT ct_inline CT_Vec2f *ct_max2fv_imm(CT_Vec2f *a, CT_Vec2f *b) {
  a->x = MAX(a->x, b->x);
  a->y = MAX(a->y, b->y);
  return a;
}

CT_EXPORT ct_inline CT_Vec2f *ct_min2fv_imm(CT_Vec2f *a, CT_Vec2f *b) {
  a->x = MIN(a->x, b->x);
  a->y = MIN(a->y, b->y);
  return a;
}

CT_EXPORT ct_inline CT_Vec2f *ct_mix2fv_imm(CT_Vec2f *a, const CT_Vec2f *b,
                                            float t) {
  a->x = ct_mixf(a->x, b->x, t);
  a->y = ct_mixf(a->y, b->y, t);
  return a;
}

CT_EXPORT ct_inline CT_Vec2f *ct_mix2fv(const CT_Vec2f *a, const CT_Vec2f *b,
                                        float t, CT_Vec2f *out) {
  return ct_mix2fv_imm(ct_set2fv(out, a), b, t);
}

CT_EXPORT ct_inline CT_Vec2f *ct_negate_imm(CT_Vec2f *v) {
  v->x = -v->x;
  v->y = -v->y;
  return v;
}

CT_EXPORT ct_inline CT_Vec2f *ct_negate(const CT_Vec2f *v, CT_Vec2f *out) {
  return ct_negate_imm(ct_set2fv(out, v));
}

CT_EXPORT ct_inline CT_Vec2f *ct_normalize2f_imm(CT_Vec2f *v, float len) {
  float m = ct_mag2f(v);
  if (m > 0.0) {
    len /= m;
    v->x *= len;
    v->y *= len;
  }
  return v;
}

CT_EXPORT ct_inline CT_Vec2f *ct_normalize2f(const CT_Vec2f *v, float len,
                                             CT_Vec2f *out) {
  return ct_normalize2f_imm(ct_set2fv(out, v), len);
}

CT_EXPORT ct_inline size_t ct_is_normalized2f(const CT_Vec2f *v) {
  return ct_deltaeqf(ct_mag2f(v), 1.f, EPS);
}

CT_EXPORT ct_inline CT_Vec2f *ct_cartesian2f_imm(CT_Vec2f *v) {
  float x = v->x * cosf(v->y);
  v->y    = v->x * sinf(v->y);
  v->x    = x;
  return v;
}

CT_EXPORT ct_inline CT_Vec2f *ct_cartesian2f(const CT_Vec2f *v, CT_Vec2f *out) {
  return ct_cartesian2f_imm(ct_set2fv(out, v));
}

CT_EXPORT ct_inline CT_Vec2f *ct_polar2f_imm(CT_Vec2f *v) {
  float x = ct_mag2f(v);
  v->y    = ct_wrapf(atan2f(v->y, v->x), TAU);
  v->x    = x;
  return v;
}

CT_EXPORT ct_inline CT_Vec2f *ct_polar2f(const CT_Vec2f *v, CT_Vec2f *out) {
  return ct_polar2f_imm(ct_set2fv(out, v));
}

CT_END_DECLS
