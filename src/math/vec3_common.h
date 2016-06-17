#pragma once

#ifdef CT_FEATURE_SSE4
#include <smmintrin.h>
#endif

#include "math/vec.h"

CT_BEGIN_DECLS

CT_Vec3f *ct_vec3f(float x, float y, float z, CT_MPool *mpool);
CT_Vec3f *ct_vec3fn(float n, CT_MPool *mpool);

CT_EXPORT ct_inline int ct_compare3fv(const CT_Vec3f *a, const CT_Vec3f *b) {
  int c = COMPARE(a->x, b->x);
  if (!c) {
    c = COMPARE(a->y, b->y);
    if (!c) {
      return COMPARE(a->z, b->z);
    }
  }
  return c;
}

CT_EXPORT ct_inline size_t ct_deltaeq3fv(const CT_Vec3f *a, const CT_Vec3f *b,
                                         float eps) {
  return (ct_deltaeqf(a->x, b->x, eps) && ct_deltaeqf(a->y, b->y, eps) &&
          ct_deltaeqf(a->z, b->z, eps));
}

CT_EXPORT ct_inline float ct_dist3fv(const CT_Vec3f *a, const CT_Vec3f *b) {
  return sqrtf(ct_distsq3fv(a, b));
}

CT_EXPORT ct_inline CT_Vec3f *ct_ceil3f_imm(CT_Vec3f *v) {
#ifdef CT_FEATURE_SSE4
  v->mmval = _mm_ceil_ps(v->mmval);
#else
  v->x = ceilf(v->x);
  v->y = ceilf(v->y);
  v->z = ceilf(v->z);
#endif
  return v;
}

CT_EXPORT ct_inline CT_Vec3f *ct_floor3f_imm(CT_Vec3f *v) {
#ifdef CT_FEATURE_SSE4
  v->mmval = _mm_floor_ps(v->mmval);
#else
  v->x = floorf(v->x);
  v->y = floorf(v->y);
  v->z = floorf(v->z);
#endif
  return v;
}

CT_EXPORT ct_inline uint32_t ct_hash3f(const CT_Vec3f *v) {
  return ct_murmur3_32(v, 3 * sizeof(float));
}

CT_EXPORT ct_inline CT_Vec3f *ct_madd3fv(const CT_Vec3f *a, const CT_Vec3f *b,
                                         const CT_Vec3f *c, CT_Vec3f *out) {
  return ct_madd3fv_imm(ct_set3fv(out, a), b, c);
}

CT_EXPORT ct_inline float ct_mag3f(const CT_Vec3f *v) {
  return sqrtf(ct_magsq3f(v));
}

CT_EXPORT ct_inline CT_Vec3f *ct_mix3fv(const CT_Vec3f *a, const CT_Vec3f *b,
                                        float t, CT_Vec3f *out) {
  return ct_mix3fv_imm(ct_set3fv(out, a), b, t);
}

CT_EXPORT ct_inline CT_Vec3f *ct_normalize3f(const CT_Vec3f *v, float len,
                                             CT_Vec3f *out) {
  return ct_normalize3f_imm(ct_set3fv(out, v), len);
}

CT_EXPORT ct_inline size_t ct_is_normalized3f(const CT_Vec3f *v) {
  return ct_deltaeqf(ct_mag3f(v), 1.f, EPS);
}

CT_EXPORT ct_inline CT_Vec3f *ct_set3fxyz(CT_Vec3f *v, float x, float y,
                                          float z) {
  v->x = x;
  v->y = y;
  v->z = z;
  return v;
}

CT_END_DECLS
