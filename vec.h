#pragma once

#include "mpool.h"

typedef struct {
  float x,y;
} CT_Vec2f;

typedef struct {
  float x,y,z;
} CT_Vec3f;

#define DECL_VEC2OP(type, type2, name)                          \
  type *name##v(type *a, type *b, CT_MPool* mpool);             \
  type *name##v_imm(type *a, type *b);                          \
  type *name##n(type *v, type2 n, CT_MPool* mpool);             \
  type *name##n_imm(type *v, type2 n);                          \
  type *name##xy(type *v, type2 x, type2 y, CT_MPool* mpool);   \
  type *name##xy_imm(type *v, type2 x, type2 y);

#define VEC2OP(type, type2, name, set, op)                              \
  type *name##v_imm(type *a, type *b) {                                 \
    a->x op##= b->x;                                                    \
    a->y op##= b->y;                                                    \
    return a;                                                           \
  }                                                                     \
                                                                        \
  type *name##v(type *a, type *b, CT_MPool* mpool) {                    \
    return name##v_imm(set##v(ALLOCATE_TYPE(mpool, type), a), b);       \
  }                                                                     \
                                                                        \
  type *name##n_imm(type *v, type2 n) {                                 \
    v->x op##= n;                                                       \
    v->y op##= n;                                                       \
    return v;                                                           \
  }                                                                     \
                                                                        \
  type *name##n(type *a, type2 n, CT_MPool* mpool) {                    \
    return name##n_imm(set##v(ALLOCATE_TYPE(mpool, type), a), n);       \
  }                                                                     \
                                                                        \
  type *name##xy_imm(type *v, type2 x, type2 y) {                       \
    v->x op##= x;                                                       \
    v->y op##= y;                                                       \
    return v;                                                           \
  }                                                                     \
                                                                        \
  type *name##xy(type *a, type2 x, type2 y, CT_MPool* mpool) {          \
    return name##xy_imm(set##v(ALLOCATE_TYPE(mpool, type), a), x, y);   \
  }


CT_Vec2f *ct_vec2f(float x, float y, CT_MPool* mpool);
CT_Vec2f *ct_set2fv(CT_Vec2f *a, CT_Vec2f *b);
CT_Vec2f *ct_set2fxy(CT_Vec2f *v, float x, float y);
DECL_VEC2OP(CT_Vec2f, float, ct_add2f)
DECL_VEC2OP(CT_Vec2f, float, ct_sub2f)
DECL_VEC2OP(CT_Vec2f, float, ct_mul2f)
DECL_VEC2OP(CT_Vec2f, float, ct_div2f)

float ct_dot2fv(CT_Vec2f *a, CT_Vec2f *b);
CT_Vec2f *ct_normalize2f_imm(CT_Vec2f *v, float len);
CT_Vec2f *ct_normalize2f(CT_Vec2f *v, float len, CT_MPool *mpool);

CT_Vec3f *ct_vec3f(float x, float y, float z, CT_MPool* mpool);
CT_Vec3f *ct_set3fv(CT_Vec3f *a, CT_Vec3f *b);
CT_Vec3f *ct_set3fxyz(CT_Vec3f *a, float x, float y, float z);
CT_Vec3f *ct_add3fv(CT_Vec3f *a, CT_Vec3f *b, CT_MPool* mpool);
CT_Vec3f *ct_add3fv_imm(CT_Vec3f *a, CT_Vec3f *b);
CT_Vec3f *ct_add3fn(CT_Vec3f *a, float n, CT_MPool* mpool);
CT_Vec3f *ct_add3fn_imm(CT_Vec3f *a, float n);
CT_Vec3f *ct_add3fxyz(CT_Vec3f *v, float x, float y, float z, CT_MPool* mpool);
CT_Vec3f *ct_add3fxyz_imm(CT_Vec3f *v, float x, float y, float z);

float ct_dot3fv(CT_Vec3f *a, CT_Vec3f *b);
CT_Vec3f *ct_cross3fv(CT_Vec3f *a, CT_Vec3f *b, CT_MPool* mpool);
CT_Vec3f *ct_normalize3f_imm(CT_Vec3f *v, float len);
CT_Vec3f *ct_normalize3f(CT_Vec3f *v, float len, CT_MPool *mpool);
