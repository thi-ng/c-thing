#pragma once

#include "vec.h"

#define VEC3OP(type, ptype, name, op)                                          \
  CT_EXPORT ct_inline type *name##v_imm(type *a, const type *b) {              \
    a->x op## = b->x;                                                          \
    a->y op## = b->y;                                                          \
    a->z op## = b->z;                                                          \
    return a;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##v(const type *a, const type *b, type *out) { \
    out->x = a->x op b->x;                                                     \
    out->y = a->y op b->y;                                                     \
    out->z = a->z op b->z;                                                     \
    return out;                                                                \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##n_imm(type *v, ptype n) {                    \
    v->x op## = n;                                                             \
    v->y op## = n;                                                             \
    v->z op## = n;                                                             \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##n(const type *v, ptype n, type *out) {       \
    out->x = v->x op n;                                                        \
    out->y = v->y op n;                                                        \
    out->z = v->z op n;                                                        \
    return out;                                                                \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##xyz_imm(type *v, ptype x, ptype y,           \
                                          ptype z) {                           \
    v->x op## = x;                                                             \
    v->y op## = y;                                                             \
    v->z op## = z;                                                             \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  CT_EXPORT ct_inline type *name##xyz(const type *v, ptype x, ptype y,         \
                                      ptype z, type *out) {                    \
    out->x = v->x op x;                                                        \
    out->y = v->y op y;                                                        \
    out->z = v->z op z;                                                        \
    return out;                                                                \
  }

typedef union {
  struct {
    float x, y, z;
  };
  float buf[3];
} CT_Vec3f;

VEC3OP(CT_Vec3f, float, ct_add3f, +)
VEC3OP(CT_Vec3f, float, ct_sub3f, -)
VEC3OP(CT_Vec3f, float, ct_mul3f, *)
VEC3OP(CT_Vec3f, float, ct_div3f, /)

CT_EXPORT ct_inline float ct_dot3fv(const CT_Vec3f *a, const CT_Vec3f *b) {
  return a->x * b->x + a->y * b->y + a->z * b->z;
}

CT_EXPORT ct_inline CT_Vec3f *ct_cross3fv_imm(CT_Vec3f *a, const CT_Vec3f *b) {
  float x = a->y * b->z - a->z * b->y;
  float y = a->z * b->x - a->x * b->z;
  a->z = a->x * b->y - a->y * b->x;
  a->y = y;
  a->x = x;
  return a;
}

CT_EXPORT ct_inline CT_Vec3f *ct_cross3fv(const CT_Vec3f *a, const CT_Vec3f *b,
                                          CT_Vec3f *out) {
  out->x = a->y * b->z - a->z * b->y;
  out->y = a->z * b->x - a->x * b->z;
  out->z = a->x * b->y - a->y * b->x;
  return out;
}

CT_EXPORT ct_inline float ct_distsq3fv(const CT_Vec3f *a, const CT_Vec3f *b) {
  float dx = a->x - b->x;
  float dy = a->y - b->y;
  float dz = a->z - b->z;
  return dx * dx + dy * dy + dz * dz;
}

CT_EXPORT ct_inline CT_Vec3f *ct_madd3fv_imm(CT_Vec3f *a, const CT_Vec3f *b,
                                             const CT_Vec3f *c) {
  a->x = a->x * b->x + c->x;
  a->y = a->y * b->y + c->y;
  a->z = a->z * b->z + c->z;
  return a;
}

CT_EXPORT ct_inline float ct_magsq3f(const CT_Vec3f *v) {
  return v->x * v->x + v->y * v->y + v->z * v->z;
}

CT_EXPORT ct_inline CT_Vec3f *ct_mix3fv_imm(CT_Vec3f *a, const CT_Vec3f *b,
                                            float t) {
  a->x = ct_mixf(a->x, b->x, t);
  a->y = ct_mixf(a->y, b->y, t);
  a->z = ct_mixf(a->z, b->z, t);
  return a;
}

CT_EXPORT ct_inline CT_Vec3f *ct_normalize3f_imm(CT_Vec3f *v, float len) {
  float m = sqrt(ct_magsq3f(v));
  if (m > 0.0) {
    len /= m;
    v->x *= len;
    v->y *= len;
    v->z *= len;
  }
  return v;
}

CT_EXPORT ct_inline CT_Vec3f *ct_set3fv(CT_Vec3f *a, const CT_Vec3f *b) {
  a->x = b->x;
  a->y = b->y;
  a->z = b->z;
  return a;
}
