#pragma once

#include "math/vec.h"

#define VEC4OP(type, ptype, name, op)                                          \
  ct_export ct_inline type *name##v_imm(type *a, const type *b) {              \
    a->x op## = b->x;                                                          \
    a->y op## = b->y;                                                          \
    a->z op## = b->z;                                                          \
    a->w op## = b->w;                                                          \
    return a;                                                                  \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##v(const type *a, const type *b, type *out) { \
    out->x = a->x op b->x;                                                     \
    out->y = a->y op b->y;                                                     \
    out->z = a->z op b->z;                                                     \
    out->w = a->w op b->w;                                                     \
    return out;                                                                \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##n_imm(type *v, ptype n) {                    \
    v->x op## = n;                                                             \
    v->y op## = n;                                                             \
    v->z op## = n;                                                             \
    v->w op## = n;                                                             \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##n(const type *v, ptype n, type *out) {       \
    out->x = v->x op n;                                                        \
    out->y = v->y op n;                                                        \
    out->z = v->z op n;                                                        \
    out->w = v->w op n;                                                        \
    return out;                                                                \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##xyzw_imm(type *v, ptype x, ptype y, ptype z, \
                                           ptype w) {                          \
    v->x op## = x;                                                             \
    v->y op## = y;                                                             \
    v->z op## = z;                                                             \
    v->w op## = w;                                                             \
    return v;                                                                  \
  }                                                                            \
                                                                               \
  ct_export ct_inline type *name##xyzw(const type *v, ptype x, ptype y,        \
                                       ptype z, ptype w, type *out) {          \
    out->x = v->x op x;                                                        \
    out->y = v->y op y;                                                        \
    out->z = v->z op z;                                                        \
    out->w = v->w op w;                                                        \
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
} CT_Vec4f;

VEC4OP(CT_Vec4f, float, ct_add4f, +)
VEC4OP(CT_Vec4f, float, ct_sub4f, -)
VEC4OP(CT_Vec4f, float, ct_mul4f, *)
VEC4OP(CT_Vec4f, float, ct_div4f, /)

ct_export ct_inline float ct_dot4fv(const CT_Vec4f *a, const CT_Vec4f *b) {
  return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
}

ct_export ct_inline float ct_distsq4fv(const CT_Vec4f *a, const CT_Vec4f *b) {
  float dx = a->x - b->x;
  float dy = a->y - b->y;
  float dz = a->z - b->z;
  float dw = a->w - b->w;
  return dx * dx + dy * dy + dz * dz + dw * dw;
}

ct_export ct_inline CT_Vec4f *ct_madd4fv_imm(CT_Vec4f *a, const CT_Vec4f *b,
                                             const CT_Vec4f *c) {
  a->x = a->x * b->x + c->x;
  a->y = a->y * b->y + c->y;
  a->z = a->z * b->z + c->z;
  a->w = a->w * b->w + c->w;
  return a;
}

ct_export ct_inline float ct_magsq4f(const CT_Vec4f *v) {
  return v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w;
}

ct_export ct_inline CT_Vec4f *ct_mix4fv_imm(CT_Vec4f *a, const CT_Vec4f *b,
                                            float t) {
  a->x = ct_mixf(a->x, b->x, t);
  a->y = ct_mixf(a->y, b->y, t);
  a->z = ct_mixf(a->z, b->z, t);
  a->w = ct_mixf(a->w, b->w, t);
  return a;
}

ct_export ct_inline CT_Vec4f *ct_normalize4f_imm(CT_Vec4f *v, float len) {
  float m = sqrt(ct_magsq4f(v));
  if (m > 0.0) {
    len /= m;
    v->x *= len;
    v->y *= len;
    v->z *= len;
    v->w *= len;
  }
  return v;
}

ct_export ct_inline CT_Vec4f *ct_set4fv(CT_Vec4f *a, const CT_Vec4f *b) {
  a->x = b->x;
  a->y = b->y;
  a->z = b->z;
  a->w = b->w;
  return a;
}

ct_export ct_inline CT_Vec4f *ct_set4fn(CT_Vec4f *v, float n) {
  v->x = n;
  v->y = n;
  v->z = n;
  v->w = n;
  return v;
}

ct_export ct_inline CT_Vec4f *ct_set4fxyzw(CT_Vec4f *v, float x, float y,
                                           float z, float w) {
  v->x = x;
  v->y = y;
  v->z = z;
  v->w = w;
  return v;
}

ct_export ct_inline CT_Vec4f *ct_set4fp(CT_Vec4f *v, const float *p) {
  v->x = p[0];
  v->y = p[1];
  v->z = p[2];
  v->w = p[3];
  return v;
}

ct_export ct_inline CT_Vec4f *ct_set4fpua(CT_Vec4f *v, const float *p) {
  v->x = p[0];
  v->y = p[1];
  v->z = p[2];
  v->w = p[3];
  return v;
}

CT_END_DECLS
