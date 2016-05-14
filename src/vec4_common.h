#pragma once

#include "vec.h"

CT_EXPORT CT_Vec4f *ct_vec4f(float x, float y, float z, float w,
                             CT_MPool *mpool);
CT_EXPORT CT_Vec4f *ct_vec4n(float n, CT_MPool *mpool);

CT_EXPORT ct_inline size_t ct_deltaeq4fv(CT_Vec4f *a, CT_Vec4f *b, float eps) {
  return (ct_deltaeqf(a->x, b->x, eps) && ct_deltaeqf(a->y, b->y, eps) &&
          ct_deltaeqf(a->z, b->z, eps) && ct_deltaeqf(a->w, b->w, eps));
}

CT_EXPORT ct_inline float ct_dist4fv(CT_Vec4f *a, CT_Vec4f *b) {
  return sqrtf(ct_distsq4fv(a, b));
}

CT_EXPORT ct_inline CT_Vec4f *ct_madd4fv(CT_Vec4f *a, CT_Vec4f *b, CT_Vec4f *c,
                                         CT_Vec4f *out) {
  return ct_madd4fv_imm(ct_set4fv(out, a), b, c);
}

CT_EXPORT ct_inline float ct_mag4f(CT_Vec4f *v) {
  return sqrtf(ct_magsq4f(v));
}

CT_EXPORT ct_inline CT_Vec4f *ct_mix4fv(CT_Vec4f *a, CT_Vec4f *b, float t,
                                        CT_Vec4f *out) {
  return ct_mix4fv_imm(ct_set4fv(out, a), b, t);
}

CT_EXPORT ct_inline CT_Vec4f *ct_normalize4f(CT_Vec4f *v, float len,
                                             CT_Vec4f *out) {
  return ct_normalize4f_imm(ct_set4fv(out, v), len);
}

CT_EXPORT ct_inline size_t ct_is_normalized4f(CT_Vec4f *v) {
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
