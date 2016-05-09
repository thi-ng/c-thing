#include <math.h>
#include <stdint.h>
#include "ct_math.h"
#include "vec.h"

CT_Vec2f *ct_vec2f(float x, float y, CT_MPool *mpool) {
    CT_Vec2f *v = ALLOCATE_TYPE(mpool, CT_Vec2f);
    if (v != NULL) {
        v->x = x;
        v->y = y;
    }
    return v;
}

CT_Vec2f *ct_set2fv(CT_Vec2f *a, CT_Vec2f *b) {
    a->x = b->x;
    a->y = b->y;
    return a;
}

CT_Vec2f *ct_set2fxy(CT_Vec2f *v, float x, float y) {
    v->x = x;
    v->y = y;
    return v;
}

uint8_t ct_deltaeq2fv(CT_Vec2f *a, CT_Vec2f *b, float eps) {
    return (ct_deltaeqf(a->x, b->x, eps) && ct_deltaeqf(a->y, b->y, eps));
}

VEC2OP(CT_Vec2f, float, ct_add2f, +)

VEC2OP(CT_Vec2f, float, ct_sub2f, -)

VEC2OP(CT_Vec2f, float, ct_mul2f, *)

VEC2OP(CT_Vec2f, float, ct_div2f, / )

CT_Vec2f *ct_madd2fv_imm(CT_Vec2f *a, CT_Vec2f *b, CT_Vec2f *c) {
    a->x = a->x * b->x + c->x;
    a->y = a->y * b->y + c->y;
    return a;
}

CT_Vec2f *ct_madd2fv(CT_Vec2f *a, CT_Vec2f *b, CT_Vec2f *c, CT_MPool *mpool) {
    return ct_madd2fv_imm(ct_set2fv(ALLOCATE_TYPE(mpool, CT_Vec2f), a), b, c);
}

float ct_dot2fv(CT_Vec2f *a, CT_Vec2f *b) {
    return a->x * b->x + a->y * b->y;
}

float ct_distsq2fv(CT_Vec2f *a, CT_Vec2f *b) {
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    return dx * dx + dy * dy;
}

float ct_dist2fv(CT_Vec2f *a, CT_Vec2f *b) {
    return sqrtf(ct_distsq2fv(a, b));
}

float ct_magsq2f(CT_Vec2f *v) {
    return v->x * v->x + v->y * v->y;
}

float ct_mag2f(CT_Vec2f *v) {
    return sqrtf(v->x * v->x + v->y * v->y);
}

CT_Vec2f *ct_mix2fv_imm(CT_Vec2f *a, CT_Vec2f *b, float t) {
    a->x = ct_mixf(a->x, b->x, t);
    a->y = ct_mixf(a->y, b->y, t);
    return a;
}

CT_Vec2f *ct_mix2fv(CT_Vec2f *a, CT_Vec2f *b, float t, CT_MPool *mpool) {
    return ct_mix2fv_imm(ct_set2fv(ALLOCATE_TYPE(mpool, CT_Vec2f), a), b, t);
}

CT_Vec2f *ct_normalize2f_imm(CT_Vec2f *v, float len) {
    float m = ct_mag2f(v);
    if (m > 0.0) {
        len /= m;
        v->x *= len;
        v->y *= len;
    }
    return v;
}

CT_Vec2f *ct_normalize2f(CT_Vec2f *v, float len, CT_MPool *mpool) {
    return ct_normalize2f_imm(ct_set2fv(ALLOCATE_TYPE(mpool, CT_Vec2f), v),
                              len);
}

uint8_t ct_is_normalized2f(CT_Vec2f *v) {
    return ct_deltaeqf(ct_mag2f(v) - 1.f, 0.f, EPS);
}

////// Vec3f

CT_Vec3f *ct_vec3f(float x, float y, float z, CT_MPool *mpool) {
    CT_Vec3f *v = ALLOCATE_TYPE(mpool, CT_Vec3f);
    if (v != NULL) {
        v->x = x;
        v->y = y;
        v->z = z;
    }
    return v;
}

CT_Vec3f *ct_set3fv(CT_Vec3f *a, CT_Vec3f *b) {
    a->x = b->x;
    a->y = b->y;
    a->z = b->z;
    return a;
}

CT_Vec3f *ct_set3fxyz(CT_Vec3f *v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
    return v;
}

uint8_t ct_deltaeq3fv(CT_Vec3f *a, CT_Vec3f *b, float eps) {
    return (ct_deltaeqf(a->x, b->x, eps) && ct_deltaeqf(a->y, b->y, eps) &&
            ct_deltaeqf(a->z, b->z, eps));
}

#ifdef CT_USE_SSE
VEC3OP_SSE(CT_Vec3f, float, ct_add3f, +)
VEC3OP_SSE(CT_Vec3f, float, ct_sub3f, -)
VEC3OP_SSE(CT_Vec3f, float, ct_mul3f, *)
VEC3OP_SSE(CT_Vec3f, float, ct_div3f, / )
#else
VEC3OP(CT_Vec3f, float, ct_add3f, +)
VEC3OP(CT_Vec3f, float, ct_sub3f, -)
VEC3OP(CT_Vec3f, float, ct_mul3f, *)
VEC3OP(CT_Vec3f, float, ct_div3f, / )
#endif

CT_Vec3f *ct_madd3fv_imm(CT_Vec3f *a, CT_Vec3f *b, CT_Vec3f *c) {
    a->x = a->x * b->x + c->x;
    a->y = a->y * b->y + c->y;
    a->z = a->z * b->z + c->z;
    return a;
}

CT_Vec3f *ct_madd3fv(CT_Vec3f *a, CT_Vec3f *b, CT_Vec3f *c, CT_MPool *mpool) {
    return ct_madd3fv_imm(ct_set3fv(ALLOCATE_TYPE(mpool, CT_Vec3f), a), b, c);
}

float ct_dot3fv(CT_Vec3f *a, CT_Vec3f *b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

CT_Vec3f *ct_cross3fv_imm(CT_Vec3f *a, CT_Vec3f *b) {
    float x = a->y * b->z - a->z * b->y;
    float y = a->z * b->x - a->x * b->z;
    a->z = a->x * b->y - a->y * b->x;
    a->y = y;
    a->x = x;
    return a;
}

CT_Vec3f *ct_cross3fv(CT_Vec3f *a, CT_Vec3f *b, CT_MPool *mpool) {
    CT_Vec3f *v = ALLOCATE_TYPE(mpool, CT_Vec3f);
    v->x = a->y * b->z - a->z * b->y;
    v->y = a->z * b->x - a->x * b->z;
    v->z = a->x * b->y - a->y * b->x;
    return v;
}

float ct_distsq3fv(CT_Vec3f *a, CT_Vec3f *b) {
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;
    return dx * dx + dy * dy + dz * dz;
}

float ct_dist3fv(CT_Vec3f *a, CT_Vec3f *b) {
    return sqrtf(ct_distsq3fv(a, b));
}

float ct_magsq3f(CT_Vec3f *v) {
    return v->x * v->x + v->y * v->y + v->z * v->z;
}

float ct_mag3f(CT_Vec3f *v) {
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

CT_Vec3f *ct_mix3fv_imm(CT_Vec3f *a, CT_Vec3f *b, float t) {
    a->x = ct_mixf(a->x, b->x, t);
    a->y = ct_mixf(a->y, b->y, t);
    a->z = ct_mixf(a->z, b->z, t);
    return a;
}

CT_Vec3f *ct_mix3fv(CT_Vec3f *a, CT_Vec3f *b, float t, CT_MPool *mpool) {
    return ct_mix3fv_imm(ct_set3fv(ALLOCATE_TYPE(mpool, CT_Vec3f), a), b, t);
}

CT_Vec3f *ct_normalize3f_imm(CT_Vec3f *v, float len) {
    float m = ct_mag3f(v);
    if (m > 0.0) {
        len /= m;
        v->x *= len;
        v->y *= len;
        v->z *= len;
    }
    return v;
}

CT_Vec3f *ct_normalize3f(CT_Vec3f *v, float len, CT_MPool *mpool) {
    return ct_normalize3f_imm(ct_set3fv(ALLOCATE_TYPE(mpool, CT_Vec3f), v),
                              len);
}

uint8_t ct_is_normalized3f(CT_Vec3f *v) {
    return ct_deltaeqf(ct_mag3f(v) - 1.f, 0.f, EPS);
}
