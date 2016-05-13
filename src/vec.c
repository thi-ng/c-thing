#include "vec.h"

CT_EXPORT CT_Vec2f *ct_vec2f(float x, float y, CT_MPool *mpool) {
  CT_Vec2f *v = ALLOCATE_TYPE(mpool, CT_Vec2f);
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
  CT_Vec3f *v = ALLOCATE_TYPE(mpool, CT_Vec3f);
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
  CT_Vec4f *v = ALLOCATE_TYPE(mpool, CT_Vec4f);
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
