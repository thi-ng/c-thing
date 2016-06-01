#include "vec.h"
#include <float.h>

CT_EXPORT CT_Vec2f *ct_vec2f(float x, float y, CT_MPool *mpool) {
  CT_Vec2f *v = CT_MP_ALLOC(mpool, CT_Vec2f);
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
  CT_Vec3f *v = CT_MP_ALLOC(mpool, CT_Vec3f);
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
  CT_Vec4f *v = CT_MP_ALLOC(mpool, CT_Vec4f);
  if (v != NULL) {
    v->x = x;
    v->y = y;
    v->z = z;
    v->w = w;
  }
  return v;
}

CT_EXPORT CT_Vec4f *ct_vec4fn(float n, CT_MPool *mpool) {
  return ct_vec4f(n, n, n, n, mpool);
}

// ---------- array ops

VEC_ARRAYOP(translate2f, CT_Vec2f, float, ct_add2fv_imm)
VEC_ARRAYOP(scale2f, CT_Vec2f, float, ct_mul2fv_imm)
VEC_ARRAYOP(translate3f, CT_Vec3f, float, ct_add3fv_imm)
VEC_ARRAYOP(scale3f, CT_Vec3f, float, ct_mul3fv_imm)
VEC_ARRAYOP(translate4f, CT_Vec4f, float, ct_add4fv_imm)
VEC_ARRAYOP(scale4f, CT_Vec4f, float, ct_mul4fv_imm)

CT_EXPORT CT_Vec2f *ct_closest_point2f(float *ptr, CT_Vec2f *p, size_t num,
                                       size_t fstride) {
  float minD = FLT_MAX;
  CT_Vec2f *closest = NULL;
  if (num > 0) {
    while (num--) {
      float d = ct_distsq2fv((CT_Vec2f *)ptr, p);
      if (d < minD) {
        closest = (CT_Vec2f *)ptr;
        minD = d;
      }
      ptr += fstride;
    }
  }
  return closest;
}

CT_EXPORT CT_Vec3f *ct_closest_point3f(float *ptr, CT_Vec3f *p, size_t num,
                                       size_t fstride) {
  float minD = FLT_MAX;
  CT_Vec3f *closest = NULL;
  if (num > 0) {
    while (num--) {
      const float d = ct_distsq3fv((CT_Vec3f *)ptr, p);
      if (d < minD) {
        closest = (CT_Vec3f *)ptr;
        minD = d;
      }
      ptr += fstride;
    }
  }
  return closest;
}
