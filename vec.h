#pragma once

#include "mpool.h"

typedef struct { float x, y; } CT_Vec2f;

typedef struct { float x, y, z; } CT_Vec3f;

#define DECL_VEC2OP(type, ptype, name)                                         \
    type *name##v(type *a, type *b, CT_MPool *mpool);                          \
    type *name##v_imm(type *a, type *b);                                       \
    type *name##n(type *v, ptype n, CT_MPool *mpool);                          \
    type *name##n_imm(type *v, ptype n);                                       \
    type *name##xy(type *v, ptype x, ptype y, CT_MPool *mpool);                \
    type *name##xy_imm(type *v, ptype x, ptype y);

#define DECL_VEC3OP(type, ptype, name)                                         \
    type *name##v(type *a, type *b, CT_MPool *mpool);                          \
    type *name##v_imm(type *a, type *b);                                       \
    type *name##n(type *v, ptype n, CT_MPool *mpool);                          \
    type *name##n_imm(type *v, ptype n);                                       \
    type *name##xyz(type *v, ptype x, ptype y, ptype z, CT_MPool *mpool);      \
    type *name##xyz_imm(type *v, ptype x, ptype y, ptype z);

#define VEC2OP(type, ptype, name, set, op)                                     \
    type *name##v_imm(type *a, type *b) {                                      \
        a->x op## = b->x;                                                      \
        a->y op## = b->y;                                                      \
        return a;                                                              \
    }                                                                          \
                                                                               \
    type *name##v(type *a, type *b, CT_MPool *mpool) {                         \
        return name##v_imm(set##v(ALLOCATE_TYPE(mpool, type), a), b);          \
    }                                                                          \
                                                                               \
    type *name##n_imm(type *v, ptype n) {                                      \
        v->x op## = n;                                                         \
        v->y op## = n;                                                         \
        return v;                                                              \
    }                                                                          \
                                                                               \
    type *name##n(type *a, ptype n, CT_MPool *mpool) {                         \
        return name##n_imm(set##v(ALLOCATE_TYPE(mpool, type), a), n);          \
    }                                                                          \
                                                                               \
    type *name##xy_imm(type *v, ptype x, ptype y) {                            \
        v->x op## = x;                                                         \
        v->y op## = y;                                                         \
        return v;                                                              \
    }                                                                          \
                                                                               \
    type *name##xy(type *a, ptype x, ptype y, CT_MPool *mpool) {               \
        return name##xy_imm(set##v(ALLOCATE_TYPE(mpool, type), a), x, y);      \
    }

#define VEC3OP(type, ptype, name, set, op)                                     \
    type *name##v_imm(type *a, type *b) {                                      \
        a->x op## = b->x;                                                      \
        a->y op## = b->y;                                                      \
        a->z op## = b->z;                                                      \
        return a;                                                              \
    }                                                                          \
                                                                               \
    type *name##v(type *a, type *b, CT_MPool *mpool) {                         \
        return name##v_imm(set##v(ALLOCATE_TYPE(mpool, type), a), b);          \
    }                                                                          \
                                                                               \
    type *name##n_imm(type *v, ptype n) {                                      \
        v->x op## = n;                                                         \
        v->y op## = n;                                                         \
        v->z op## = n;                                                         \
        return v;                                                              \
    }                                                                          \
                                                                               \
    type *name##n(type *a, ptype n, CT_MPool *mpool) {                         \
        return name##n_imm(set##v(ALLOCATE_TYPE(mpool, type), a), n);          \
    }                                                                          \
                                                                               \
    type *name##xyz_imm(type *v, ptype x, ptype y, ptype z) {                  \
        v->x op## = x;                                                         \
        v->y op## = y;                                                         \
        v->z op## = z;                                                         \
        return v;                                                              \
    }                                                                          \
                                                                               \
    type *name##xyz(type *a, ptype x, ptype y, ptype z, CT_MPool *mpool) {     \
        return name##xyz_imm(set##v(ALLOCATE_TYPE(mpool, type), a), x, y, z);  \
    }

CT_Vec2f *ct_vec2f(float x, float y, CT_MPool *mpool);
CT_Vec2f *ct_set2fv(CT_Vec2f *a, CT_Vec2f *b);
CT_Vec2f *ct_set2fxy(CT_Vec2f *v, float x, float y);
DECL_VEC2OP(CT_Vec2f, float, ct_add2f)
DECL_VEC2OP(CT_Vec2f, float, ct_sub2f)
DECL_VEC2OP(CT_Vec2f, float, ct_mul2f)
DECL_VEC2OP(CT_Vec2f, float, ct_div2f)

uint8_t ct_deltaeq2fv(CT_Vec2f *a, CT_Vec2f *b, float eps);
float ct_dot2fv(CT_Vec2f *a, CT_Vec2f *b);
float ct_distsq2fv(CT_Vec2f *a, CT_Vec2f *b);
float ct_dist2fv(CT_Vec2f *a, CT_Vec2f *b);
float ct_magsq2f(CT_Vec2f *v);
float ct_mag2f(CT_Vec2f *v);
CT_Vec2f *ct_normalize2f_imm(CT_Vec2f *v, float len);
CT_Vec2f *ct_normalize2f(CT_Vec2f *v, float len, CT_MPool *mpool);
uint8_t ct_is_normalized2f(CT_Vec2f *v);

CT_Vec3f *ct_vec3f(float x, float y, float z, CT_MPool *mpool);
CT_Vec3f *ct_set3fv(CT_Vec3f *a, CT_Vec3f *b);
CT_Vec3f *ct_set3fxyz(CT_Vec3f *a, float x, float y, float z);
DECL_VEC3OP(CT_Vec3f, float, ct_add3f)
DECL_VEC3OP(CT_Vec3f, float, ct_sub3f)
DECL_VEC3OP(CT_Vec3f, float, ct_mul3f)
DECL_VEC3OP(CT_Vec3f, float, ct_div3f)

uint8_t ct_deltaeq3fv(CT_Vec3f *a, CT_Vec3f *b, float eps);
float ct_dot3fv(CT_Vec3f *a, CT_Vec3f *b);
float ct_distsq3fv(CT_Vec3f *a, CT_Vec3f *b);
float ct_dist3fv(CT_Vec3f *a, CT_Vec3f *b);
CT_Vec3f *ct_cross3fv(CT_Vec3f *a, CT_Vec3f *b, CT_MPool *mpool);
float ct_magsq3f(CT_Vec3f *v);
float ct_mag3f(CT_Vec3f *v);
CT_Vec3f *ct_normalize3f_imm(CT_Vec3f *v, float len);
CT_Vec3f *ct_normalize3f(CT_Vec3f *v, float len, CT_MPool *mpool);
uint8_t ct_is_normalized3f(CT_Vec3f *v);
