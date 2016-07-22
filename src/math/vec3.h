#pragma once

#include "math/vec.h"

#define VEC3OP(type, ptype, name, op)                                          \
  ct_export ct_inline type *name##v_imm(type *a, const type *b) {              \
    a->x op## = b->x;                                                          \
    a->y op## = b->y;                                                          \
    a->z op## = b->z;                                                          \
    return a;                                                                  \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##v(const type *a, const type *b, type *out) { \
    out->x = a->x op b->x;                                                     \
    out->y = a->y op b->y;                                                     \
    out->z = a->z op b->z;                                                     \
    return out;                                                                \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##n_imm(type *v, ptype n) {                    \
    v->x op## = n;                                                             \
    v->y op## = n;                                                             \
    v->z op## = n;                                                             \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##n(const type *v, ptype n, type *out) {       \
    out->x = v->x op n;                                                        \
    out->y = v->y op n;                                                        \
    out->z = v->z op n;                                                        \
    return out;                                                                \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##xyz_imm(type *v, ptype x, ptype y,           \
                                          ptype z) {                           \
    v->x op## = x;                                                             \
    v->y op## = y;                                                             \
    v->z op## = z;                                                             \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##xyz(const type *v, ptype x, ptype y,         \
                                      ptype z, type *out) {                    \
    out->x = v->x op x;                                                        \
    out->y = v->y op y;                                                        \
    out->z = v->z op z;                                                        \
    return out;                                                                \
  }

CT_BEGIN_DECLS

typedef union {
  struct {
    float x, y, z;
  };
  CT_Vec2f xy;
  float buf[3];
} CT_Vec3f;

VEC3OP(CT_Vec3f, float, ct_add3f, +)
VEC3OP(CT_Vec3f, float, ct_sub3f, -)
VEC3OP(CT_Vec3f, float, ct_mul3f, *)
VEC3OP(CT_Vec3f, float, ct_div3f, /)

ct_export ct_inline float ct_dot3fv(const CT_Vec3f *a, const CT_Vec3f *b) {
  return a->x * b->x + a->y * b->y + a->z * b->z;
}

ct_export ct_inline CT_Vec3f *ct_cross3fv_imm(CT_Vec3f *a, const CT_Vec3f *b) {
  float x = a->y * b->z - a->z * b->y;
  float y = a->z * b->x - a->x * b->z;
  a->z    = a->x * b->y - a->y * b->x;
  a->y    = y;
  a->x    = x;
  return a;
}

ct_export ct_inline CT_Vec3f *ct_cross3fv(const CT_Vec3f *a,
                                          const CT_Vec3f *b,
                                          CT_Vec3f *out) {
  out->x = a->y * b->z - a->z * b->y;
  out->y = a->z * b->x - a->x * b->z;
  out->z = a->x * b->y - a->y * b->x;
  return out;
}

ct_export ct_inline float ct_distsq3fv(const CT_Vec3f *a, const CT_Vec3f *b) {
  float dx = a->x - b->x;
  float dy = a->y - b->y;
  float dz = a->z - b->z;
  return dx * dx + dy * dy + dz * dz;
}

ct_export ct_inline CT_Vec3f *ct_madd3fv_imm(CT_Vec3f *a,
                                             const CT_Vec3f *b,
                                             const CT_Vec3f *c) {
  a->x = a->x * b->x + c->x;
  a->y = a->y * b->y + c->y;
  a->z = a->z * b->z + c->z;
  return a;
}

ct_export ct_inline float ct_magsq3f(const CT_Vec3f *v) {
  return v->x * v->x + v->y * v->y + v->z * v->z;
}

ct_export ct_inline CT_Vec3f *ct_max3fv_imm(CT_Vec3f *a, CT_Vec3f *b) {
  a->x = MAX(a->x, b->x);
  a->y = MAX(a->y, b->y);
  a->z = MAX(a->z, b->z);
  return a;
}

ct_export ct_inline CT_Vec3f *ct_min3fv_imm(CT_Vec3f *a, CT_Vec3f *b) {
  a->x = MIN(a->x, b->x);
  a->y = MIN(a->y, b->y);
  a->z = MIN(a->z, b->z);
  return a;
}

ct_export ct_inline CT_Vec3f *ct_mix3fv_imm(CT_Vec3f *a,
                                            const CT_Vec3f *b,
                                            float t) {
  a->x = ct_mixf(a->x, b->x, t);
  a->y = ct_mixf(a->y, b->y, t);
  a->z = ct_mixf(a->z, b->z, t);
  return a;
}

ct_export ct_inline CT_Vec3f *ct_negate3f_imm(CT_Vec3f *v) {
  v->x = -v->x;
  v->y = -v->y;
  v->z = -v->z;
  return v;
}

ct_export ct_inline CT_Vec3f *ct_normalize3f_imm(CT_Vec3f *v, float len) {
  float m = sqrt(ct_magsq3f(v));
  if (m > 0.0) {
    len /= m;
    v->x *= len;
    v->y *= len;
    v->z *= len;
  }
  return v;
}

ct_export ct_inline CT_Vec3f *ct_set3fv(CT_Vec3f *a, const CT_Vec3f *b) {
  a->x = b->x;
  a->y = b->y;
  a->z = b->z;
  return a;
}

ct_export ct_inline CT_Vec3f *ct_set3fn(CT_Vec3f *v, float n) {
  v->x = n;
  v->y = n;
  v->z = n;
  return v;
}

ct_export ct_inline CT_Vec3f *ct_set3fxyz(CT_Vec3f *v,
                                          float x,
                                          float y,
                                          float z) {
  v->x = x;
  v->y = y;
  v->z = z;
  return v;
}

ct_export ct_inline CT_Vec3f *ct_set3fp(CT_Vec3f *v, const float *p) {
  v->x = p[0];
  v->y = p[1];
  v->z = p[2];
  return v;
}

ct_export ct_inline CT_Vec3f *ct_set3fpua(CT_Vec3f *v, const float *p) {
  v->x = p[0];
  v->y = p[1];
  v->z = p[2];
  return v;
}

CT_END_DECLS
