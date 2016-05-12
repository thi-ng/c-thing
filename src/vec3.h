#pragma once

#include "vec.h"

#define VEC3OP(type, ptype, name, op)                                          \
    CT_EXPORT ct_inline type *name##v_imm(type *a, type *b) {                  \
        a->x op## = b->x;                                                      \
        a->y op## = b->y;                                                      \
        a->z op## = b->z;                                                      \
        return a;                                                              \
    }                                                                          \
                                                                               \
    CT_EXPORT ct_inline type *name##v(type *a, type *b, CT_MPool *mpool) {     \
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        v->x = a->x op b->x;                                                   \
        v->y = a->y op b->y;                                                   \
        v->z = a->z op b->z;                                                   \
        return v;                                                              \
    }                                                                          \
                                                                               \
    CT_EXPORT ct_inline type *name##n_imm(type *v, ptype n) {                  \
        v->x op## = n;                                                         \
        v->y op## = n;                                                         \
        v->z op## = n;                                                         \
        return v;                                                              \
    }                                                                          \
                                                                               \
    CT_EXPORT ct_inline type *name##n(type *a, ptype n, CT_MPool *mpool) {     \
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        v->x = a->x op n;                                                      \
        v->y = a->y op n;                                                      \
        v->z = a->z op n;                                                      \
        return v;                                                              \
    }                                                                          \
                                                                               \
    CT_EXPORT ct_inline type *name##xyz_imm(type *v, ptype x, ptype y,         \
                                            ptype z) {                         \
        v->x op## = x;                                                         \
        v->y op## = y;                                                         \
        v->z op## = z;                                                         \
        return v;                                                              \
    }                                                                          \
                                                                               \
    CT_EXPORT ct_inline type *name##xyz(type *a, ptype x, ptype y, ptype z,    \
                                        CT_MPool *mpool) {                     \
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        v->x = a->x op x;                                                      \
        v->y = a->y op y;                                                      \
        v->z = a->z op z;                                                      \
        return v;                                                              \
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
VEC3OP(CT_Vec3f, float, ct_div3f, / )

CT_EXPORT ct_inline CT_Vec3f *ct_madd3fv_imm(CT_Vec3f *a, CT_Vec3f *b,
                                             CT_Vec3f *c) {
    a->x = a->x * b->x + c->x;
    a->y = a->y * b->y + c->y;
    a->z = a->z * b->z + c->z;
    return a;
}

CT_EXPORT ct_inline float ct_dot3fv(CT_Vec3f *a, CT_Vec3f *b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

CT_EXPORT ct_inline CT_Vec3f *ct_cross3fv_imm(CT_Vec3f *a, CT_Vec3f *b) {
    float x = a->y * b->z - a->z * b->y;
    float y = a->z * b->x - a->x * b->z;
    a->z = a->x * b->y - a->y * b->x;
    a->y = y;
    a->x = x;
    return a;
}

CT_EXPORT ct_inline CT_Vec3f *ct_cross3fv(CT_Vec3f *a, CT_Vec3f *b,
                                          CT_MPool *mpool) {
    CT_Vec3f *v = ALLOCATE_TYPE(mpool, CT_Vec3f);
    v->x = a->y * b->z - a->z * b->y;
    v->y = a->z * b->x - a->x * b->z;
    v->z = a->x * b->y - a->y * b->x;
    return v;
}

CT_EXPORT ct_inline float ct_distsq3fv(CT_Vec3f *a, CT_Vec3f *b) {
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;
    return dx * dx + dy * dy + dz * dz;
}

CT_EXPORT ct_inline float ct_magsq3f(CT_Vec3f *v) {
    return v->x * v->x + v->y * v->y + v->z * v->z;
}

CT_EXPORT ct_inline CT_Vec3f *ct_mix3fv_imm(CT_Vec3f *a, CT_Vec3f *b, float t) {
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

CT_EXPORT ct_inline CT_Vec3f *ct_set3fv(CT_Vec3f *a, CT_Vec3f *b) {
    a->x = b->x;
    a->y = b->y;
    a->z = b->z;
    return a;
}
