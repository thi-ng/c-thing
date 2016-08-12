#pragma once

#ifdef CT_FEATURE_SSE4
#include <smmintrin.h>
#endif

#include "math/vec.h"

CT_BEGIN_DECLS

CT_Vec3f *ct_vec3f(float x, float y, float z, CT_MPool *mpool);
CT_Vec3f *ct_vec3fn(float n, CT_MPool *mpool);

ct_export ct_inline int ct_compare3fv(const void *a, const void *b) {
  CT_Vec3f *va = (CT_Vec3f *)a;
  CT_Vec3f *vb = (CT_Vec3f *)b;
  int c        = COMPARE(va->x, vb->x);
  if (!c) {
    c = COMPARE(va->y, vb->y);
    if (!c) {
      return COMPARE(va->z, vb->z);
    }
  }
  return c;
}

ct_export ct_inline int ct_compare3fv_xyz(const void *a, const void *b) {
  CT_Vec3f *va = (CT_Vec3f *)a;
  CT_Vec3f *vb = (CT_Vec3f *)b;
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
  return 0;
}

ct_export ct_inline int ct_compare3fv_yxz(const void *a, const void *b) {
  CT_Vec3f *va = (CT_Vec3f *)a;
  CT_Vec3f *vb = (CT_Vec3f *)b;
  if (va->y < vb->y)
    return -1;
  if (va->y > vb->y)
    return 1;
  if (va->x < vb->x)
    return -1;
  if (va->x > vb->x)
    return 1;
  if (va->z < vb->z)
    return -1;
  if (va->z > vb->z)
    return 1;
  return 0;
}

ct_export ct_inline int ct_compare3fv_yzx(const void *a, const void *b) {
  CT_Vec3f *va = (CT_Vec3f *)a;
  CT_Vec3f *vb = (CT_Vec3f *)b;
  if (va->y < vb->y)
    return -1;
  if (va->y > vb->y)
    return 1;
  if (va->z < vb->z)
    return -1;
  if (va->z > vb->z)
    return 1;
  if (va->x < vb->x)
    return -1;
  if (va->x > vb->x)
    return 1;
  return 0;
}

ct_export ct_inline int ct_compare3fv_zyx(const void *a, const void *b) {
  CT_Vec3f *va = (CT_Vec3f *)a;
  CT_Vec3f *vb = (CT_Vec3f *)b;
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

ct_export ct_inline int ct_compare3fv_zxy(const void *a, const void *b) {
  CT_Vec3f *va = (CT_Vec3f *)a;
  CT_Vec3f *vb = (CT_Vec3f *)b;
  if (va->z < vb->z)
    return -1;
  if (va->z > vb->z)
    return 1;
  if (va->x < vb->x)
    return -1;
  if (va->x > vb->x)
    return 1;
  if (va->y < vb->y)
    return -1;
  if (va->y > vb->y)
    return 1;
  return 0;
}

ct_export ct_inline size_t ct_deltaeq3fv(const CT_Vec3f *a,
                                         const CT_Vec3f *b,
                                         float eps) {
  return (ct_deltaeqf(a->x, b->x, eps) && ct_deltaeqf(a->y, b->y, eps) &&
          ct_deltaeqf(a->z, b->z, eps));
}

ct_export ct_inline float ct_dist3fv(const CT_Vec3f *a, const CT_Vec3f *b) {
  return sqrtf(ct_distsq3fv(a, b));
}

ct_export ct_inline float ct_dist3fv_manhatten(const CT_Vec3f *a,
                                               const CT_Vec3f *b) {
  return fabs(a->x - b->x) + fabs(a->y - b->y) + fabs(a->z - b->z);
}

ct_export ct_inline float ct_dist3fv_minkowski(const CT_Vec3f *a,
                                               const CT_Vec3f *b,
                                               float p) {
  return powf(powf(fabs(a->x - b->x), p) + powf(fabs(a->y - b->y), p) +
                  powf(fabs(a->z - b->z), p),
              1 / p);
}

ct_export ct_inline CT_Vec3f *ct_ceil3f_imm(CT_Vec3f *v) {
#ifdef CT_FEATURE_SSE4
  v->mmval = _mm_ceil_ps(v->mmval);
#else
  v->x = ceilf(v->x);
  v->y = ceilf(v->y);
  v->z = ceilf(v->z);
#endif
  return v;
}

ct_export ct_inline CT_Vec3f *ct_floor3f_imm(CT_Vec3f *v) {
#ifdef CT_FEATURE_SSE4
  v->mmval = _mm_floor_ps(v->mmval);
#else
  v->x = floorf(v->x);
  v->y = floorf(v->y);
  v->z = floorf(v->z);
#endif
  return v;
}

ct_export ct_inline uint32_t ct_hash3f(const CT_Vec3f *v) {
  return ct_murmur3_32(v, 3 * sizeof(float));
}

ct_export ct_inline CT_Vec3f *ct_madd3fv(const CT_Vec3f *a,
                                         const CT_Vec3f *b,
                                         const CT_Vec3f *c,
                                         CT_Vec3f *out) {
  return ct_madd3fv_imm(ct_set3fv(out, a), b, c);
}

ct_export ct_inline float ct_mag3f(const CT_Vec3f *v) {
  return sqrtf(ct_magsq3f(v));
}

ct_export ct_inline CT_Vec3f *ct_mix3fv(const CT_Vec3f *a,
                                        const CT_Vec3f *b,
                                        float t,
                                        CT_Vec3f *out) {
  return ct_mix3fv_imm(ct_set3fv(out, a), b, t);
}

ct_export ct_inline CT_Vec3f *ct_negate3f(const CT_Vec3f *v, CT_Vec3f *out) {
  return ct_negate3f_imm(ct_set3fv(out, v));
}

ct_export ct_inline CT_Vec3f *ct_normalize3f(const CT_Vec3f *v,
                                             float len,
                                             CT_Vec3f *out) {
  return ct_normalize3f_imm(ct_set3fv(out, v), len);
}

ct_export ct_inline size_t ct_is_normalized3f(const CT_Vec3f *v) {
  return ct_deltaeqf(ct_mag3f(v), 1.f, EPS);
}

ct_export ct_inline CT_Vec3f *ct_limit3f_imm(CT_Vec3f *v, float len) {
  float m = ct_mag3f(v);
  if (m > len) {
    len /= m;
    v->x *= len;
    v->y *= len;
  }
  return v;
}

ct_export ct_inline CT_Vec3f *ct_orthonormal3fv(const CT_Vec3f *a,
                                                const CT_Vec3f *b,
                                                const CT_Vec3f *c,
                                                CT_Vec3f *out) {
  CT_Vec3f t;
  return ct_cross3fv_imm(ct_sub3fv(c, a, out), ct_sub3fv(b, a, &t));
}
CT_END_DECLS
