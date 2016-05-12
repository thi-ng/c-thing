#pragma once

#include "vec.h"

#define VEC2_SWIZZLE(type, ptype, name, suffix, i, j)                          \
    CT_EXPORT ct_inline type *ct_##name##suffix(void *a, type *b) {            \
        ptype *v = (ptype *)a;                                                 \
        ptype x = v[i];                                                        \
        ptype y = v[j];                                                        \
        b->x = x;                                                              \
        b->y = y;                                                              \
        return b;                                                              \
    }

#define VEC2_SWIZZLE_N(type, ptype, name, suffix, i)                           \
    CT_EXPORT ct_inline type *ct_##name##suffix(void *a, type *b) {            \
        ptype v = ((ptype *)a)[i];                                             \
        b->x = v;                                                              \
        b->y = v;                                                              \
        return b;                                                              \
    }

#define VEC3_SWIZZLE2(type, ptype, name, suffix, i, j, k)                      \
    CT_EXPORT ct_inline type *ct_##name##suffix(void *a, type *b) {            \
        ptype *v = (ptype *)a;                                                 \
        ptype x = v[i];                                                        \
        ptype y = v[j];                                                        \
        ptype z = v[k];                                                        \
        b->x = x;                                                              \
        b->y = y;                                                              \
        b->z = z;                                                              \
        return b;                                                              \
    }

#ifdef CT_USE_SSE

#define VEC3_SWIZZLE(type, ptype, name, suffix, i, j, k)                       \
    CT_EXPORT ct_inline type *ct_##name##suffix(void *a, type *b) {            \
        type *v = (type *)a;                                                   \
        b->mmval =                                                             \
            _mm_shuffle_ps(v->mmval, v->mmval, _MM_SHUFFLE(0, k, j, i));       \
        return b;                                                              \
    }

#define VEC3_SWIZZLE_N(type, ptype, name, suffix, i)                           \
    CT_EXPORT ct_inline type *ct_##name##suffix(void *a, type *b) {            \
        ptype *v = (ptype *)a;                                                 \
        __m128 mv = {v[i], v[i], v[i], 0.f};                                   \
        b->mmval = mv;                                                         \
        return b;                                                              \
    }

#define VEC4_SWIZZLE(type, ptype, name, suffix, i, j, k, l)                    \
    CT_EXPORT ct_inline type *ct_##name##suffix(void *a, type *b) {          \
        type *v = (type *)a;                                                   \
        b->mmval =                                                             \
            _mm_shuffle_ps(v->mmval, v->mmval, _MM_SHUFFLE(l, k, j, i));       \
        return b;                                                              \
    }

#define VEC4_SWIZZLE_N(type, ptype, name, suffix, i)                           \
    CT_EXPORT ct_inline type *ct_##name##suffix(void *a, type *b) {            \
        ptype *v = (ptype *)a;                                                 \
        __m128 mv = {v[i], v[i], v[i], v[i]};                                  \
        b->mmval = mv;                                                         \
        return b;                                                              \
    }

#else

#define VEC3_SWIZZLE(type, ptype, name, suffix, i, j, k)                       \
    CT_EXPORT ct_inline type *ct_##name##suffix(void *a, type *b) {            \
        ptype *v = (ptype *)a;                                                 \
        ptype x = v[i];                                                        \
        ptype y = v[j];                                                        \
        ptype z = v[k];                                                        \
        b->x = x;                                                              \
        b->y = y;                                                              \
        b->z = z;                                                              \
        return b;                                                              \
    }

#define VEC3_SWIZZLE_N(type, ptype, name, suffix, i)                           \
    CT_EXPORT ct_inline type *ct_##name##suffix(void *a, type *b) {            \
      ptype v = ((ptype *)a)[i];                                           \
        b->x = v;                                                              \
        b->y = v;                                                              \
        b->z = v;                                                              \
        return b;                                                              \
    }

#define VEC4_SWIZZLE(type, ptype, name, suffix, i, j, k, l)               \
    CT_EXPORT ct_inline type *ct_##name##suffix(void *a, type *b) {            \
        ptype *v = (ptype *)a;                                                 \
        ptype x = v[i];                                                        \
        ptype y = v[j];                                                        \
        ptype z = v[k];                                                        \
        ptype w = v[l];                                                        \
        b->x = x;                                                              \
        b->y = y;                                                              \
        b->z = z;                                                              \
        b->w = w;                                                              \
        return b;                                                              \
    }

#define VEC4_SWIZZLE_N(type, ptype, name, suffix, i)                           \
    CT_EXPORT ct_inline type *ct_##name##suffix(void *a, type *b) {            \
        ptype v = ((ptype *)a)[i];                                             \
        b->x = v;                                                              \
        b->y = v;                                                              \
        b->z = v;                                                              \
        b->w = v;                                                              \
        return b;                                                              \
    }

#endif

// ct_xxf(void, 2f)
// ct_xxf(void, 2f)
// ct_xxx2f(2f, 3f)
// ct_xxxf(void, 3f)
// ct_www4f(4f, 3f)

VEC2_SWIZZLE_N(CT_Vec2f, float, xx, f, 0)
VEC2_SWIZZLE_N(CT_Vec2f, float, yy, f, 1)
VEC2_SWIZZLE_N(CT_Vec2f, float, zz, f, 2)
VEC2_SWIZZLE_N(CT_Vec2f, float, ww, f, 3)
VEC3_SWIZZLE_N(CT_Vec3f, float, xxx, f, 0)
VEC3_SWIZZLE_N(CT_Vec3f, float, yyy, f, 1)
VEC3_SWIZZLE_N(CT_Vec3f, float, zzz, f, 2)
VEC3_SWIZZLE_N(CT_Vec3f, float, www, f, 3)
VEC4_SWIZZLE_N(CT_Vec4f, float, xxxx, f, 0)
VEC4_SWIZZLE_N(CT_Vec4f, float, yyyy, f, 1)
VEC4_SWIZZLE_N(CT_Vec4f, float, zzzz, f, 2)
VEC4_SWIZZLE_N(CT_Vec4f, float, wwww, f, 3)

VEC2_SWIZZLE(CT_Vec2f, float, xy, f, 0, 1)
VEC2_SWIZZLE(CT_Vec2f, float, xz, f, 0, 2)
VEC2_SWIZZLE(CT_Vec2f, float, xw, f, 0, 3)
VEC2_SWIZZLE(CT_Vec2f, float, yx, f, 1, 0)
VEC2_SWIZZLE(CT_Vec2f, float, yz, f, 1, 2)
VEC2_SWIZZLE(CT_Vec2f, float, yw, f, 1, 3)
VEC2_SWIZZLE(CT_Vec2f, float, zx, f, 2, 0)
VEC2_SWIZZLE(CT_Vec2f, float, zy, f, 2, 1)
VEC2_SWIZZLE(CT_Vec2f, float, zw, f, 2, 3)
VEC2_SWIZZLE(CT_Vec2f, float, wx, f, 3, 0)
VEC2_SWIZZLE(CT_Vec2f, float, wy, f, 3, 1)
VEC2_SWIZZLE(CT_Vec2f, float, wz, f, 3, 2)

VEC3_SWIZZLE2(CT_Vec3f, float, xxy, f, 0, 0, 1)
VEC3_SWIZZLE2(CT_Vec3f, float, xyx, f, 0, 1, 0)
VEC3_SWIZZLE2(CT_Vec3f, float, xyy, f, 0, 1, 1)
VEC3_SWIZZLE2(CT_Vec3f, float, yxx, f, 1, 0, 0)
VEC3_SWIZZLE2(CT_Vec3f, float, yxy, f, 1, 0, 1)
VEC3_SWIZZLE2(CT_Vec3f, float, yyx, f, 1, 1, 0)

VEC3_SWIZZLE(CT_Vec3f, float, xxz, f, 0, 0, 2)
VEC3_SWIZZLE(CT_Vec3f, float, xxw, f, 0, 0, 3)
VEC3_SWIZZLE(CT_Vec3f, float, xyz, f, 0, 1, 2)
VEC3_SWIZZLE(CT_Vec3f, float, xyw, f, 0, 1, 3)
VEC3_SWIZZLE(CT_Vec3f, float, xzx, f, 0, 2, 0)
VEC3_SWIZZLE(CT_Vec3f, float, xzy, f, 0, 2, 1)
VEC3_SWIZZLE(CT_Vec3f, float, xzz, f, 0, 2, 2)
VEC3_SWIZZLE(CT_Vec3f, float, xzw, f, 0, 2, 3)
VEC3_SWIZZLE(CT_Vec3f, float, xwx, f, 0, 3, 0)
VEC3_SWIZZLE(CT_Vec3f, float, xwy, f, 0, 3, 1)
VEC3_SWIZZLE(CT_Vec3f, float, xwz, f, 0, 3, 2)
VEC3_SWIZZLE(CT_Vec3f, float, xww, f, 0, 3, 3)
VEC3_SWIZZLE(CT_Vec3f, float, yxz, f, 1, 0, 2)
VEC3_SWIZZLE(CT_Vec3f, float, yxw, f, 1, 0, 3)
VEC3_SWIZZLE(CT_Vec3f, float, yyz, f, 1, 1, 2)
VEC3_SWIZZLE(CT_Vec3f, float, yyw, f, 1, 1, 3)
VEC3_SWIZZLE(CT_Vec3f, float, yzx, f, 1, 2, 0)
VEC3_SWIZZLE(CT_Vec3f, float, yzy, f, 1, 2, 1)
VEC3_SWIZZLE(CT_Vec3f, float, yzz, f, 1, 2, 2)
VEC3_SWIZZLE(CT_Vec3f, float, yzw, f, 1, 2, 3)
VEC3_SWIZZLE(CT_Vec3f, float, ywx, f, 1, 3, 0)
VEC3_SWIZZLE(CT_Vec3f, float, ywy, f, 1, 3, 1)
VEC3_SWIZZLE(CT_Vec3f, float, ywz, f, 1, 3, 2)
VEC3_SWIZZLE(CT_Vec3f, float, yww, f, 1, 3, 3)
VEC3_SWIZZLE(CT_Vec3f, float, zxx, f, 2, 0, 0)
VEC3_SWIZZLE(CT_Vec3f, float, zxy, f, 2, 0, 1)
VEC3_SWIZZLE(CT_Vec3f, float, zxz, f, 2, 0, 2)
VEC3_SWIZZLE(CT_Vec3f, float, zxw, f, 2, 0, 3)
VEC3_SWIZZLE(CT_Vec3f, float, zyx, f, 2, 1, 0)
VEC3_SWIZZLE(CT_Vec3f, float, zyy, f, 2, 1, 1)
VEC3_SWIZZLE(CT_Vec3f, float, zyz, f, 2, 1, 2)
VEC3_SWIZZLE(CT_Vec3f, float, zyw, f, 2, 1, 3)
VEC3_SWIZZLE(CT_Vec3f, float, zzx, f, 2, 2, 0)
VEC3_SWIZZLE(CT_Vec3f, float, zzy, f, 2, 2, 1)
VEC3_SWIZZLE(CT_Vec3f, float, zzw, f, 2, 2, 3)
VEC3_SWIZZLE(CT_Vec3f, float, zwx, f, 2, 3, 0)
VEC3_SWIZZLE(CT_Vec3f, float, zwy, f, 2, 3, 1)
VEC3_SWIZZLE(CT_Vec3f, float, zwz, f, 2, 3, 2)
VEC3_SWIZZLE(CT_Vec3f, float, zww, f, 2, 3, 3)
VEC3_SWIZZLE(CT_Vec3f, float, wxx, f, 3, 0, 0)
VEC3_SWIZZLE(CT_Vec3f, float, wxy, f, 3, 0, 1)
VEC3_SWIZZLE(CT_Vec3f, float, wxz, f, 3, 0, 2)
VEC3_SWIZZLE(CT_Vec3f, float, wxw, f, 3, 0, 3)
VEC3_SWIZZLE(CT_Vec3f, float, wyx, f, 3, 1, 0)
VEC3_SWIZZLE(CT_Vec3f, float, wyy, f, 3, 1, 1)
VEC3_SWIZZLE(CT_Vec3f, float, wyz, f, 3, 1, 2)
VEC3_SWIZZLE(CT_Vec3f, float, wyw, f, 3, 1, 3)
VEC3_SWIZZLE(CT_Vec3f, float, wzx, f, 3, 2, 0)
VEC3_SWIZZLE(CT_Vec3f, float, wzy, f, 3, 2, 1)
VEC3_SWIZZLE(CT_Vec3f, float, wzz, f, 3, 2, 2)
VEC3_SWIZZLE(CT_Vec3f, float, wzw, f, 3, 2, 3)
VEC3_SWIZZLE(CT_Vec3f, float, wwx, f, 3, 3, 0)
VEC3_SWIZZLE(CT_Vec3f, float, wwy, f, 3, 3, 1)
VEC3_SWIZZLE(CT_Vec3f, float, wwz, f, 3, 3, 2)

VEC4_SWIZZLE(CT_Vec4f, float, xxxy, f, 0, 0, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xxxz, f, 0, 0, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xxxw, f, 0, 0, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, xxyx, f, 0, 0, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xxyy, f, 0, 0, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xxyz, f, 0, 0, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xxyw, f, 0, 0, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, xxzx, f, 0, 0, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xxzy, f, 0, 0, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xxzz, f, 0, 0, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xxzw, f, 0, 0, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, xxwx, f, 0, 0, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xxwy, f, 0, 0, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xxwz, f, 0, 0, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xxww, f, 0, 0, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, xyxx, f, 0, 1, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xyxy, f, 0, 1, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xyxz, f, 0, 1, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xyxw, f, 0, 1, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, xyyx, f, 0, 1, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xyyy, f, 0, 1, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xyyz, f, 0, 1, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xyyw, f, 0, 1, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, xyzx, f, 0, 1, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xyzy, f, 0, 1, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xyzz, f, 0, 1, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xyzw, f, 0, 1, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, xywx, f, 0, 1, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xywy, f, 0, 1, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xywz, f, 0, 1, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xyww, f, 0, 1, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, xzxx, f, 0, 2, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xzxy, f, 0, 2, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xzxz, f, 0, 2, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xzxw, f, 0, 2, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, xzyx, f, 0, 2, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xzyy, f, 0, 2, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xzyz, f, 0, 2, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xzyw, f, 0, 2, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, xzzx, f, 0, 2, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xzzy, f, 0, 2, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xzzz, f, 0, 2, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xzzw, f, 0, 2, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, xzwx, f, 0, 2, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xzwy, f, 0, 2, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xzwz, f, 0, 2, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xzww, f, 0, 2, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, xwxx, f, 0, 3, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xwxy, f, 0, 3, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xwxz, f, 0, 3, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xwxw, f, 0, 3, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, xwyx, f, 0, 3, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xwyy, f, 0, 3, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xwyz, f, 0, 3, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xwyw, f, 0, 3, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, xwzx, f, 0, 3, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xwzy, f, 0, 3, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xwzz, f, 0, 3, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xwzw, f, 0, 3, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, xwwx, f, 0, 3, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, xwwy, f, 0, 3, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, xwwz, f, 0, 3, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, xwww, f, 0, 3, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, yxxx, f, 1, 0, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, yxxy, f, 1, 0, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, yxxz, f, 1, 0, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, yxxw, f, 1, 0, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, yxyx, f, 1, 0, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, yxyy, f, 1, 0, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, yxyz, f, 1, 0, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, yxyw, f, 1, 0, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, yxzx, f, 1, 0, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, yxzy, f, 1, 0, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, yxzz, f, 1, 0, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, yxzw, f, 1, 0, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, yxwx, f, 1, 0, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, yxwy, f, 1, 0, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, yxwz, f, 1, 0, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, yxww, f, 1, 0, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, yyxx, f, 1, 1, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, yyxy, f, 1, 1, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, yyxz, f, 1, 1, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, yyxw, f, 1, 1, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, yyyx, f, 1, 1, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, yyyz, f, 1, 1, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, yyyw, f, 1, 1, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, yyzx, f, 1, 1, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, yyzy, f, 1, 1, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, yyzz, f, 1, 1, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, yyzw, f, 1, 1, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, yywx, f, 1, 1, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, yywy, f, 1, 1, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, yywz, f, 1, 1, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, yyww, f, 1, 1, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, yzxx, f, 1, 2, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, yzxy, f, 1, 2, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, yzxz, f, 1, 2, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, yzxw, f, 1, 2, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, yzyx, f, 1, 2, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, yzyy, f, 1, 2, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, yzyz, f, 1, 2, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, yzyw, f, 1, 2, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, yzzx, f, 1, 2, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, yzzy, f, 1, 2, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, yzzz, f, 1, 2, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, yzzw, f, 1, 2, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, yzwx, f, 1, 2, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, yzwy, f, 1, 2, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, yzwz, f, 1, 2, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, yzww, f, 1, 2, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, ywxx, f, 1, 3, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, ywxy, f, 1, 3, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, ywxz, f, 1, 3, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, ywxw, f, 1, 3, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, ywyx, f, 1, 3, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, ywyy, f, 1, 3, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, ywyz, f, 1, 3, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, ywyw, f, 1, 3, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, ywzx, f, 1, 3, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, ywzy, f, 1, 3, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, ywzz, f, 1, 3, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, ywzw, f, 1, 3, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, ywwx, f, 1, 3, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, ywwy, f, 1, 3, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, ywwz, f, 1, 3, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, ywww, f, 1, 3, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, zxxx, f, 2, 0, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zxxy, f, 2, 0, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zxxz, f, 2, 0, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zxxw, f, 2, 0, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, zxyx, f, 2, 0, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zxyy, f, 2, 0, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zxyz, f, 2, 0, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zxyw, f, 2, 0, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, zxzx, f, 2, 0, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zxzy, f, 2, 0, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zxzz, f, 2, 0, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zxzw, f, 2, 0, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, zxwx, f, 2, 0, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zxwy, f, 2, 0, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zxwz, f, 2, 0, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zxww, f, 2, 0, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, zyxx, f, 2, 1, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zyxy, f, 2, 1, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zyxz, f, 2, 1, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zyxw, f, 2, 1, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, zyyx, f, 2, 1, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zyyy, f, 2, 1, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zyyz, f, 2, 1, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zyyw, f, 2, 1, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, zyzx, f, 2, 1, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zyzy, f, 2, 1, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zyzz, f, 2, 1, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zyzw, f, 2, 1, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, zywx, f, 2, 1, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zywy, f, 2, 1, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zywz, f, 2, 1, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zyww, f, 2, 1, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, zzxx, f, 2, 2, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zzxy, f, 2, 2, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zzxz, f, 2, 2, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zzxw, f, 2, 2, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, zzyx, f, 2, 2, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zzyy, f, 2, 2, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zzyz, f, 2, 2, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zzyw, f, 2, 2, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, zzzx, f, 2, 2, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zzzy, f, 2, 2, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zzzw, f, 2, 2, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, zzwx, f, 2, 2, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zzwy, f, 2, 2, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zzwz, f, 2, 2, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zzww, f, 2, 2, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, zwxx, f, 2, 3, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zwxy, f, 2, 3, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zwxz, f, 2, 3, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zwxw, f, 2, 3, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, zwyx, f, 2, 3, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zwyy, f, 2, 3, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zwyz, f, 2, 3, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zwyw, f, 2, 3, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, zwzx, f, 2, 3, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zwzy, f, 2, 3, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zwzz, f, 2, 3, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zwzw, f, 2, 3, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, zwwx, f, 2, 3, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, zwwy, f, 2, 3, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, zwwz, f, 2, 3, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, zwww, f, 2, 3, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, wxxx, f, 3, 0, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wxxy, f, 3, 0, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wxxz, f, 3, 0, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wxxw, f, 3, 0, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, wxyx, f, 3, 0, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wxyy, f, 3, 0, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wxyz, f, 3, 0, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wxyw, f, 3, 0, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, wxzx, f, 3, 0, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wxzy, f, 3, 0, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wxzz, f, 3, 0, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wxzw, f, 3, 0, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, wxwx, f, 3, 0, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wxwy, f, 3, 0, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wxwz, f, 3, 0, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wxww, f, 3, 0, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, wyxx, f, 3, 1, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wyxy, f, 3, 1, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wyxz, f, 3, 1, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wyxw, f, 3, 1, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, wyyx, f, 3, 1, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wyyy, f, 3, 1, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wyyz, f, 3, 1, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wyyw, f, 3, 1, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, wyzx, f, 3, 1, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wyzy, f, 3, 1, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wyzz, f, 3, 1, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wyzw, f, 3, 1, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, wywx, f, 3, 1, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wywy, f, 3, 1, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wywz, f, 3, 1, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wyww, f, 3, 1, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, wzxx, f, 3, 2, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wzxy, f, 3, 2, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wzxz, f, 3, 2, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wzxw, f, 3, 2, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, wzyx, f, 3, 2, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wzyy, f, 3, 2, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wzyz, f, 3, 2, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wzyw, f, 3, 2, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, wzzx, f, 3, 2, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wzzy, f, 3, 2, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wzzz, f, 3, 2, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wzzw, f, 3, 2, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, wzwx, f, 3, 2, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wzwy, f, 3, 2, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wzwz, f, 3, 2, 3, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wzww, f, 3, 2, 3, 3)

VEC4_SWIZZLE(CT_Vec4f, float, wwxx, f, 3, 3, 0, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wwxy, f, 3, 3, 0, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wwxz, f, 3, 3, 0, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wwxw, f, 3, 3, 0, 3)
VEC4_SWIZZLE(CT_Vec4f, float, wwyx, f, 3, 3, 1, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wwyy, f, 3, 3, 1, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wwyz, f, 3, 3, 1, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wwyw, f, 3, 3, 1, 3)
VEC4_SWIZZLE(CT_Vec4f, float, wwzx, f, 3, 3, 2, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wwzy, f, 3, 3, 2, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wwzz, f, 3, 3, 2, 2)
VEC4_SWIZZLE(CT_Vec4f, float, wwzw, f, 3, 3, 2, 3)
VEC4_SWIZZLE(CT_Vec4f, float, wwwx, f, 3, 3, 3, 0)
VEC4_SWIZZLE(CT_Vec4f, float, wwwy, f, 3, 3, 3, 1)
VEC4_SWIZZLE(CT_Vec4f, float, wwwz, f, 3, 3, 3, 2)
