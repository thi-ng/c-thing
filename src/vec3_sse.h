#pragma once

#include <xmmintrin.h>
#include "vec.h"

#define VEC3OP_SSE(type, ptype, name, op)                                      \
    CT_EXPORT ct_inline type *name##v_imm(type *a, type *b) {                  \
        a->mmval op## = b->mmval;                                              \
        return a;                                                              \
    }                                                                          \
                                                                               \
    CT_EXPORT ct_inline type *name##v(type *a, type *b, CT_MPool *mpool) {     \
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        v->mmval = a->mmval op b->mmval;                                       \
        return v;                                                              \
    }                                                                          \
                                                                               \
    CT_EXPORT ct_inline type *name##n_imm(type *v, ptype n) {                  \
        __m128 b = {n, n, n, n};                                               \
        v->mmval op## = b;                                                     \
        return v;                                                              \
    }                                                                          \
                                                                               \
    CT_EXPORT ct_inline type *name##n(type *a, ptype n, CT_MPool *mpool) {     \
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        __m128 b = {n, n, n, n};                                               \
        v->mmval = a->mmval op b;                                              \
        return v;                                                              \
    }                                                                          \
                                                                               \
    CT_EXPORT ct_inline type *name##xyz_imm(type *v, ptype x, ptype y,         \
                                            ptype z) {                         \
        __m128 b = {x, y, z, 0.f};                                             \
        v->mmval op## = b;                                                     \
        return v;                                                              \
    }                                                                          \
                                                                               \
    CT_EXPORT ct_inline type *name##xyz(type *a, ptype x, ptype y, ptype z,    \
                                        CT_MPool *mpool) {                     \
        type *v = ALLOCATE_TYPE(mpool, type);                                  \
        __m128 b = {x, y, z, 0.f};                                             \
        v->mmval = a->mmval op b;                                              \
        return v;                                                              \
    }

#define VEC3_SWIZZLE_SSE(type, name, suffix, i, j, k)                          \
    CT_EXPORT ct_inline type *ct_##name##suffix(type *v) {                     \
        v->mmval =                                                             \
            _mm_shuffle_ps(v->mmval, v->mmval, _MM_SHUFFLE(0, k, j, i));       \
        return v;                                                              \
    }

typedef union {
    struct {
        float x, y, z;
    };
    float buf[3];
    __m128 mmval;
} CT_Vec3f;

VEC3OP_SSE(CT_Vec3f, float, ct_add3f, +)
VEC3OP_SSE(CT_Vec3f, float, ct_sub3f, -)
VEC3OP_SSE(CT_Vec3f, float, ct_mul3f, *)
VEC3OP_SSE(CT_Vec3f, float, ct_div3f, / )

VEC3_SWIZZLE_SSE(CT_Vec3f, xxx, f, 0, 0, 0)
VEC3_SWIZZLE_SSE(CT_Vec3f, xxy, f, 0, 0, 1)
VEC3_SWIZZLE_SSE(CT_Vec3f, xxz, f, 0, 0, 2)
VEC3_SWIZZLE_SSE(CT_Vec3f, xyx, f, 0, 1, 0)
VEC3_SWIZZLE_SSE(CT_Vec3f, xyy, f, 0, 1, 1)
VEC3_SWIZZLE_SSE(CT_Vec3f, xyz, f, 0, 1, 2)
VEC3_SWIZZLE_SSE(CT_Vec3f, xzx, f, 0, 2, 0)
VEC3_SWIZZLE_SSE(CT_Vec3f, xzy, f, 0, 2, 1)
VEC3_SWIZZLE_SSE(CT_Vec3f, xzz, f, 0, 2, 2)

VEC3_SWIZZLE_SSE(CT_Vec3f, yxx, f, 1, 0, 0)
VEC3_SWIZZLE_SSE(CT_Vec3f, yxy, f, 1, 0, 1)
VEC3_SWIZZLE_SSE(CT_Vec3f, yxz, f, 1, 0, 2)
VEC3_SWIZZLE_SSE(CT_Vec3f, yyx, f, 1, 1, 0)
VEC3_SWIZZLE_SSE(CT_Vec3f, yyy, f, 1, 1, 1)
VEC3_SWIZZLE_SSE(CT_Vec3f, yyz, f, 1, 1, 2)
VEC3_SWIZZLE_SSE(CT_Vec3f, yzx, f, 1, 2, 0)
VEC3_SWIZZLE_SSE(CT_Vec3f, yzy, f, 1, 2, 1)
VEC3_SWIZZLE_SSE(CT_Vec3f, yzz, f, 1, 2, 2)

VEC3_SWIZZLE_SSE(CT_Vec3f, zxx, f, 2, 0, 0)
VEC3_SWIZZLE_SSE(CT_Vec3f, zxy, f, 2, 0, 1)
VEC3_SWIZZLE_SSE(CT_Vec3f, zxz, f, 2, 0, 2)
VEC3_SWIZZLE_SSE(CT_Vec3f, zyx, f, 2, 1, 0)
VEC3_SWIZZLE_SSE(CT_Vec3f, zyy, f, 2, 1, 1)
VEC3_SWIZZLE_SSE(CT_Vec3f, zyz, f, 2, 1, 2)
VEC3_SWIZZLE_SSE(CT_Vec3f, zzx, f, 2, 2, 0)
VEC3_SWIZZLE_SSE(CT_Vec3f, zzy, f, 2, 2, 1)
VEC3_SWIZZLE_SSE(CT_Vec3f, zzz, f, 2, 2, 2)

CT_EXPORT ct_inline float ct_dot3fv(CT_Vec3f *a, CT_Vec3f *b) {
    __m128 d = a->mmval * b->mmval;
    return d[0] + d[1] + d[2];
}

CT_EXPORT ct_inline CT_Vec3f *ct_cross3fv_imm(CT_Vec3f *a, CT_Vec3f *b) {
    __m128 v = _mm_sub_ps(
        _mm_mul_ps(a->mmval,
                   _mm_shuffle_ps(b->mmval, b->mmval, _MM_SHUFFLE(3, 0, 2, 1))),
        _mm_mul_ps(b->mmval, _mm_shuffle_ps(a->mmval, a->mmval,
                                            _MM_SHUFFLE(3, 0, 2, 1))));
    a->mmval = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 0, 2, 1));
    return a;
}

CT_EXPORT ct_inline float ct_distsq3fv(CT_Vec3f *a, CT_Vec3f *b) {
    __m128 d = a->mmval - b->mmval;
    d *= d;
    return d[0] + d[1] + d[2];
}

CT_EXPORT ct_inline CT_Vec3f *ct_madd3fv_imm(CT_Vec3f *a, CT_Vec3f *b,
                                             CT_Vec3f *c) {
    a->mmval = a->mmval * b->mmval + c->mmval;
    return a;
}

CT_EXPORT ct_inline float ct_magsq3f(CT_Vec3f *v) {
    __m128 d = v->mmval * v->mmval;
    return d[0] + d[1] + d[2];
}

CT_EXPORT ct_inline CT_Vec3f *ct_mix3fv_imm(CT_Vec3f *a, CT_Vec3f *b, float t) {
    __m128 mt = {t, t, t, 0.f};
    a->mmval += (b->mmval - a->mmval) * mt;
    return a;
}

CT_EXPORT ct_inline CT_Vec3f *ct_normalize3f_imm(CT_Vec3f *v, float len) {
    float m = sqrt(ct_magsq3f(v));
    if (m > 0.0) {
        len /= m;
        __m128 ml = {len, len, len, 0.f};
        v->mmval *= ml;
    }
    return v;
}

CT_EXPORT ct_inline CT_Vec3f *ct_set3fv(CT_Vec3f *a, CT_Vec3f *b) {
    a->mmval = b->mmval;
    return a;
}
