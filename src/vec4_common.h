#pragma once

#include "vec.h"

CT_Vec4f *ct_vec4f(float x, float y, float z, float w, CT_MPool *mpool);
CT_Vec4f *ct_vec4fn(float n, CT_MPool *mpool);

CT_EXPORT ct_inline int ct_compare4fv(const CT_Vec4f *a, const CT_Vec4f *b) {
  int c = COMPARE(a->x, b->x);
  if (!c) {
    c = COMPARE(a->y, b->y);
    if (!c) {
      c = COMPARE(a->z, b->z);
      if (!c) {
        return COMPARE(a->w, b->w);
      }
    }
  }
  return c;
}

CT_EXPORT ct_inline size_t ct_deltaeq4fv(const CT_Vec4f *a, const CT_Vec4f *b,
                                         float eps) {
  return (ct_deltaeqf(a->x, b->x, eps) && ct_deltaeqf(a->y, b->y, eps) &&
          ct_deltaeqf(a->z, b->z, eps) && ct_deltaeqf(a->w, b->w, eps));
}

CT_EXPORT ct_inline float ct_dist4fv(const CT_Vec4f *a, const CT_Vec4f *b) {
  return sqrtf(ct_distsq4fv(a, b));
}

CT_EXPORT ct_inline uint32_t ct_hash4f(const CT_Vec4f *v) {
  return ct_murmur3_32(v, 4 * sizeof(float));
}

CT_EXPORT ct_inline CT_Vec4f *ct_madd4fv(const CT_Vec4f *a, const CT_Vec4f *b,
                                         const CT_Vec4f *c, CT_Vec4f *out) {
  return ct_madd4fv_imm(ct_set4fv(out, a), b, c);
}

CT_EXPORT ct_inline float ct_mag4f(const CT_Vec4f *v) {
  return sqrtf(ct_magsq4f(v));
}

CT_EXPORT ct_inline CT_Vec4f *ct_mix4fv(const CT_Vec4f *a, const CT_Vec4f *b,
                                        float t, CT_Vec4f *out) {
  return ct_mix4fv_imm(ct_set4fv(out, a), b, t);
}

CT_EXPORT ct_inline CT_Vec4f *ct_normalize4f(const CT_Vec4f *v, float len,
                                             CT_Vec4f *out) {
  return ct_normalize4f_imm(ct_set4fv(out, v), len);
}

CT_EXPORT ct_inline size_t ct_is_normalized4f(const CT_Vec4f *v) {
  return ct_deltaeqf(ct_mag4f(v), 1.f, EPS);
}

CT_EXPORT ct_inline CT_Vec4f *ct_set4fxyzw(CT_Vec4f *v, float x, float y,
                                           float z, float w) {
  v->x = x;
  v->y = y;
  v->z = z;
  v->w = w;
  return v;
}

CT_EXPORT ct_inline CT_Vec4f *ct_set4fn(CT_Vec4f *v, float n) {
  v->x = n;
  v->y = n;
  v->z = n;
  v->w = n;
  return v;
}
