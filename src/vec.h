#pragma once

#include "config.h"
#include "mpool.h"

typedef struct { float x, y; } CT_Vec2f;

#ifdef CT_USE_SSE
#include <xmmintrin.h>
typedef union {
    struct {
        float x, y, z;
    };
    __m128 mmval;
} CT_Vec3f;
typedef union {
    struct {
        float x, y, z, w;
    };
    __m128 mmval;
} CT_Vec4f;
#else
typedef struct { float x, y, z; } CT_Vec3f;
typedef struct { float x, y, z, w; } CT_Vec4f;
#endif

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

#define VEC2OP(type, ptype, name, op)                                          \
    type *name##v_imm(type *a, type *b) {                                      \
        a->x op## = b->x;                                                      \
        a->y op## = b->y;                                                      \
        return a;                                                              \
    }                                                                          \
                                                                               \
    type *name##v(type *a, type *b, CT_MPool *mpool) {                         \
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        v->x = a->x op b->x;                                                   \
        v->y = a->y op b->y;                                                   \
        return v;                                                              \
    }                                                                          \
                                                                               \
    type *name##n_imm(type *v, ptype n) {                                      \
        v->x op## = n;                                                         \
        v->y op## = n;                                                         \
        return v;                                                              \
    }                                                                          \
                                                                               \
    type *name##n(type *a, ptype n, CT_MPool *mpool) {                         \
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        v->x = a->x op n;                                                      \
        v->y = a->y op n;                                                      \
        return v;                                                              \
    }                                                                          \
                                                                               \
    type *name##xy_imm(type *v, ptype x, ptype y) {                            \
        v->x op## = x;                                                         \
        v->y op## = y;                                                         \
        return v;                                                              \
    }                                                                          \
                                                                               \
    type *name##xy(type *a, ptype x, ptype y, CT_MPool *mpool) {               \
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        v->x = a->x op x;                                                      \
        v->y = a->y op y;                                                      \
        return v;                                                              \
    }

#define VEC3OP(type, ptype, name, op)                                          \
    type *name##v_imm(type *a, type *b) {                                      \
        a->x op## = b->x;                                                      \
        a->y op## = b->y;                                                      \
        a->z op## = b->z;                                                      \
        return a;                                                              \
    }                                                                          \
                                                                               \
    type *name##v(type *a, type *b, CT_MPool *mpool) {                         \
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        v->x = a->x op b->x;                                                   \
        v->y = a->y op b->y;                                                   \
        v->z = a->z op b->z;                                                   \
        return v;                                                              \
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
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        v->x = a->x op n;                                                      \
        v->y = a->y op n;                                                      \
        v->z = a->z op n;                                                      \
        return v;                                                              \
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
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        v->x = a->x op x;                                                      \
        v->y = a->y op y;                                                      \
        v->z = a->z op z;                                                      \
        return v;                                                              \
    }

#define VEC3OP_SSE(type, ptype, name, op)                                      \
    type *name##v_imm(type *a, type *b) {                                      \
        a->mmval op## = b->mmval;                                              \
        return a;                                                              \
    }                                                                          \
                                                                               \
    type *name##v(type *a, type *b, CT_MPool *mpool) {                         \
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        v->mmval = a->mmval op b->mmval;                                       \
        return v;                                                              \
    }                                                                          \
                                                                               \
    type *name##n_imm(type *v, ptype n) {                                      \
        __m128 b = {n, n, n, n};                                               \
        v->mmval op## = b;                                                     \
        return v;                                                              \
    }                                                                          \
                                                                               \
    type *name##n(type *a, ptype n, CT_MPool *mpool) {                         \
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        __m128 b = {n, n, n, n};                                               \
        v->mmval = a->mmval op b;                                              \
        return v;                                                              \
    }                                                                          \
                                                                               \
    type *name##xyz_imm(type *v, ptype x, ptype y, ptype z) {                  \
        __m128 b = {x, y, z, 0.f};                                             \
        v->mmval op## = b;                                                     \
        return v;                                                              \
    }                                                                          \
                                                                               \
    type *name##xyz(type *a, ptype x, ptype y, ptype z, CT_MPool *mpool) {     \
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        __m128 b = {x, y, z, 0.f};                                             \
        v->mmval = a->mmval op b;                                              \
        return v;                                                              \
    }

CT_Vec2f *ct_vec2f(float x, float y, CT_MPool *mpool);
CT_Vec2f *ct_set2fv(CT_Vec2f *a, CT_Vec2f *b);
CT_Vec2f *ct_set2fxy(CT_Vec2f *v, float x, float y);
DECL_VEC2OP(CT_Vec2f, float, ct_add2f)
DECL_VEC2OP(CT_Vec2f, float, ct_sub2f)
DECL_VEC2OP(CT_Vec2f, float, ct_mul2f)
DECL_VEC2OP(CT_Vec2f, float, ct_div2f)
CT_Vec2f *ct_madd2fv_imm(CT_Vec2f *a, CT_Vec2f *b, CT_Vec2f *c);
CT_Vec2f *ct_madd2fv(CT_Vec2f *a, CT_Vec2f *b, CT_Vec2f *c, CT_MPool *mpool);
uint8_t ct_deltaeq2fv(CT_Vec2f *a, CT_Vec2f *b, float eps);
float ct_dot2fv(CT_Vec2f *a, CT_Vec2f *b);
float ct_distsq2fv(CT_Vec2f *a, CT_Vec2f *b);
float ct_dist2fv(CT_Vec2f *a, CT_Vec2f *b);
float ct_magsq2f(CT_Vec2f *v);
float ct_mag2f(CT_Vec2f *v);
CT_Vec2f *ct_mix2fv_imm(CT_Vec2f *a, CT_Vec2f *b, float t);
CT_Vec2f *ct_mix2fv(CT_Vec2f *a, CT_Vec2f *b, float t, CT_MPool *mpool);
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
CT_Vec3f *ct_madd3fv_imm(CT_Vec3f *a, CT_Vec3f *b, CT_Vec3f *c);
CT_Vec3f *ct_madd3fv(CT_Vec3f *a, CT_Vec3f *b, CT_Vec3f *c, CT_MPool *mpool);
uint8_t ct_deltaeq3fv(CT_Vec3f *a, CT_Vec3f *b, float eps);
float ct_dot3fv(CT_Vec3f *a, CT_Vec3f *b);
float ct_distsq3fv(CT_Vec3f *a, CT_Vec3f *b);
float ct_dist3fv(CT_Vec3f *a, CT_Vec3f *b);
CT_Vec3f *ct_cross3fv(CT_Vec3f *a, CT_Vec3f *b, CT_MPool *mpool);
float ct_magsq3f(CT_Vec3f *v);
float ct_mag3f(CT_Vec3f *v);
CT_Vec3f *ct_mix3fv_imm(CT_Vec3f *a, CT_Vec3f *b, float t);
CT_Vec3f *ct_mix3fv(CT_Vec3f *a, CT_Vec3f *b, float t, CT_MPool *mpool);
CT_Vec3f *ct_normalize3f_imm(CT_Vec3f *v, float len);
CT_Vec3f *ct_normalize3f(CT_Vec3f *v, float len, CT_MPool *mpool);
uint8_t ct_is_normalized3f(CT_Vec3f *v);
