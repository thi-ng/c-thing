#pragma once

#ifdef CT_FEATURE_SSE4
#include <smmintrin.h>
#endif

#include "math/vec.h"

CT_BEGIN_DECLS

CT_Vec4f *ct_vec4f(float x, float y, float z, float w, CT_MPool *mpool);
CT_Vec4f *ct_vec4fn(float n, CT_MPool *mpool);

ct_export ct_inline int ct_compare4fv_xyzw(const void *a, const void *b) {
  CT_Vec4f *va = (CT_Vec4f *)a;
  CT_Vec4f *vb = (CT_Vec4f *)b;
  if (va->x < vb->x)
    return -1;
  if (va->x > vb->x)
    return 1;
  if (va->y < vb->y)
    return -1;
  if (va->y > vb->y)
    return 1;
  if (va->z < vb->z)
    return -1;
  if (va->z > vb->z)
    return 1;
  if (va->w < vb->w)
    return -1;
  if (va->w > vb->w)
    return 1;
  return 0;
}

ct_export ct_inline int ct_compare4fv_wzyx(const void *a, const void *b) {
  CT_Vec4f *va = (CT_Vec4f *)a;
  CT_Vec4f *vb = (CT_Vec4f *)b;
  if (va->w < vb->w)
    return -1;
  if (va->w > vb->w)
    return 1;
  if (va->z < vb->z)
    return -1;
  if (va->z > vb->z)
    return 1;
  if (va->y < vb->y)
    return -1;
  if (va->y > vb->y)
    return 1;
  if (va->x < vb->x)
    return -1;
  if (va->x > vb->x)
    return 1;
  return 0;
}

ct_export ct_inline size_t ct_deltaeq4fv(const CT_Vec4f *a,
                                         const CT_Vec4f *b,
                                         float eps) {
  return (ct_deltaeqf(a->x, b->x, eps) && ct_deltaeqf(a->y, b->y, eps) &&
          ct_deltaeqf(a->z, b->z, eps) && ct_deltaeqf(a->w, b->w, eps));
}

ct_export ct_inline float ct_dist4fv(const CT_Vec4f *a, const CT_Vec4f *b) {
  return sqrtf(ct_distsq4fv(a, b));
}

ct_export ct_inline CT_Vec4f *ct_ceil4f_imm(CT_Vec4f *v) {
#ifdef CT_FEATURE_SSE4
  v->mmval = _mm_ceil_ps(v->mmval);
#else
  v->x = ceilf(v->x);
  v->y = ceilf(v->y);
  v->z = ceilf(v->z);
  v->w = ceilf(v->w);
#endif
  return v;
}

ct_export ct_inline CT_Vec4f *ct_floor4f_imm(CT_Vec4f *v) {
#ifdef CT_FEATURE_SSE4
  v->mmval = _mm_floor_ps(v->mmval);
#else
  v->x = floorf(v->x);
  v->y = floorf(v->y);
  v->z = floorf(v->z);
  v->w = floorf(v->w);
#endif
  return v;
}

ct_export ct_inline uint32_t ct_hash4f(const CT_Vec4f *v) {
  return ct_murmur3_32(v, 4 * sizeof(float));
}

ct_export ct_inline CT_Vec4f *ct_madd4fv(const CT_Vec4f *a,
                                         const CT_Vec4f *b,
                                         const CT_Vec4f *c,
                                         CT_Vec4f *out) {
  return ct_madd4fv_imm(ct_set4fv(out, a), b, c);
}

ct_export ct_inline float ct_mag4f(const CT_Vec4f *v) {
  return sqrtf(ct_magsq4f(v));
}

ct_export ct_inline CT_Vec4f *ct_mix4fv(const CT_Vec4f *a,
                                        const CT_Vec4f *b,
                                        float t,
                                        CT_Vec4f *out) {
  return ct_mix4fv_imm(ct_set4fv(out, a), b, t);
}

ct_export ct_inline CT_Vec4f *ct_normalize4f(const CT_Vec4f *v,
                                             float len,
                                             CT_Vec4f *out) {
  return ct_normalize4f_imm(ct_set4fv(out, v), len);
}

ct_export ct_inline size_t ct_is_normalized4f(const CT_Vec4f *v) {
  return ct_deltaeqf(ct_mag4f(v), 1.f, EPS);
}

ct_export ct_inline CT_Vec4f *ct_limit4f_imm(CT_Vec4f *v, float len) {
  float m = ct_mag4f(v);
  if (m > len) {
    len /= m;
    v->x *= len;
    v->y *= len;
  }
  return v;
}

CT_END_DECLS
