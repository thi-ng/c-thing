#include "vec.h"

CT_EXPORT CT_Vec2f *ct_vec2f(float x, float y, CT_MPool *mpool) {
  CT_Vec2f *v = CT_MP_ALLOC_STD(mpool, CT_Vec2f);
  if (v != NULL) {
    v->x = x;
    v->y = y;
  }
  return v;
}

CT_EXPORT CT_Vec2f *ct_vec2fn(float n, CT_MPool *mpool) {
  return ct_vec2f(n, n, mpool);
}

CT_EXPORT CT_Vec3f *ct_vec3f(float x, float y, float z, CT_MPool *mpool) {
  CT_Vec3f *v = CT_MP_ALLOC_STD(mpool, CT_Vec3f);
  if (v != NULL) {
    v->x = x;
    v->y = y;
    v->z = z;
  }
  return v;
}

CT_EXPORT CT_Vec3f *ct_vec3fn(float n, CT_MPool *mpool) {
  return ct_vec3f(n, n, n, mpool);
}

CT_EXPORT CT_Vec4f *ct_vec4f(float x, float y, float z, float w,
                             CT_MPool *mpool) {
  CT_Vec4f *v = CT_MP_ALLOC_STD(mpool, CT_Vec4f);
  if (v != NULL) {
    v->x = x;
    v->y = y;
    v->z = z;
    v->w = w;
  }
  return v;
}

CT_EXPORT CT_Vec4f *ct_vec4n(float n, CT_MPool *mpool) {
  return ct_vec4f(n, n, n, n, mpool);
}

CT_EXPORT void ct_translate2f(float *ptr, CT_Vec2f *t, size_t num,
                              size_t fstride) {
  while (num--) {
    ct_add2fv_imm((CT_Vec2f *)ptr, t);
    ptr += fstride;
  }
}

CT_EXPORT void ct_scale2f(float *ptr, CT_Vec2f *s, size_t num, size_t fstride) {
  while (num--) {
    ct_mul2fv_imm((CT_Vec2f *)ptr, s);
    ptr += fstride;
  }
}

CT_EXPORT void ct_translate3f(float *ptr, CT_Vec3f *t, size_t num,
                              size_t fstride) {
  while (num--) {
    ct_add3fv_imm((CT_Vec3f *)ptr, t);
    ptr += fstride;
  }
}

CT_EXPORT void ct_scale3f(float *ptr, CT_Vec3f *s, size_t num, size_t fstride) {
  while (num--) {
    ct_mul3fv_imm((CT_Vec3f *)ptr, s);
    ptr += fstride;
  }
}

CT_EXPORT void ct_translate4f(float *ptr, CT_Vec4f *t, size_t num,
                              size_t fstride) {
  while (num--) {
    ct_add4fv_imm((CT_Vec4f *)ptr, t);
    ptr += fstride;
  }
}

CT_EXPORT void ct_scale4f(float *ptr, CT_Vec4f *s, size_t num, size_t fstride) {
  while (num--) {
    ct_mul4fv_imm((CT_Vec4f *)ptr, s);
    ptr += fstride;
  }
}
