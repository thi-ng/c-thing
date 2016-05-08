#include "vec.h"

CT_Vec2f *ct_vec2f(float x, float y, CT_MPool* mpool) {
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

CT_Vec2f *ct_add2fv(CT_Vec2f *a, CT_Vec2f *b, CT_MPool* mpool) {
  CT_Vec2f *v = ALLOCATE_TYPE(mpool, CT_Vec2f);
  if (v != NULL) {
    v->x = a->x + b->x;
    v->y = a->y + b->y;
  }
  return v;
}

CT_Vec2f *ct_add2fv_imm(CT_Vec2f *a, CT_Vec2f *b) {
  a->x += b->x;
  a->y += b->y;
  return a;
}

CT_Vec2f *ct_add2fn(CT_Vec2f *a, float n, CT_MPool* mpool) {
  CT_Vec2f *v = ALLOCATE_TYPE(mpool, CT_Vec2f);
  if (v != NULL) {
    v->x = a->x + n;
    v->y = a->y + n;
  }
  return v;
}

CT_Vec2f *ct_add2fn_imm(CT_Vec2f *v, float n) {
  v->x += n;
  v->y += n;
  return v;
}

CT_Vec2f *ct_add2fxy(CT_Vec2f *a, float x, float y, CT_MPool* mpool) {
  CT_Vec2f *v = ALLOCATE_TYPE(mpool, CT_Vec2f);
  if (v != NULL) {
    v->x = a->x + x;
    v->y = a->y + y;
  }
  return v;
}

CT_Vec2f *ct_add2fxy_imm(CT_Vec2f *v, float x, float y) {
  v->x += x;
  v->y += y;
  return v;
}

float ct_dot2fv(CT_Vec2f *a, CT_Vec2f *b) {
  return a->x * b->x + a->y * b->y;
}

////// Vec3f

CT_Vec3f *ct_vec3f(float x, float y, float z, CT_MPool* mpool) {
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

CT_Vec3f *ct_add3fv(CT_Vec3f *a, CT_Vec3f *b, CT_MPool* mpool) {
  CT_Vec3f *v = ALLOCATE_TYPE(mpool, CT_Vec3f);
  if (v != NULL) {
    v->x = a->x + b->x;
    v->y = a->y + b->y;
    v->z = a->z + b->z;
  }
  return v;
}

CT_Vec3f *ct_add3fv_imm(CT_Vec3f *a, CT_Vec3f *b) {
  a->x += b->x;
  a->y += b->y;
  a->z += b->z;
  return a;
}

CT_Vec3f *ct_add3fn(CT_Vec3f *a, float n, CT_MPool* mpool) {
  CT_Vec3f *v = ALLOCATE_TYPE(mpool, CT_Vec3f);
  if (v != NULL) {
    v->x = a->x + n;
    v->y = a->y + n;
    v->z = a->z + n;
  }
  return v;
}

CT_Vec3f *ct_add3fn_imm(CT_Vec3f *v, float n) {
  v->x += n;
  v->y += n;
  v->z += n;
  return v;
}

CT_Vec3f *ct_add3fxy(CT_Vec3f *a, float x, float y, float z, CT_MPool* mpool) {
  CT_Vec3f *v = ALLOCATE_TYPE(mpool, CT_Vec3f);
  if (v != NULL) {
    v->x = a->x + x;
    v->y = a->y + y;
    v->z = a->z + z;
  }
  return v;
}

CT_Vec3f *ct_add3fxy_imm(CT_Vec3f *v, float x, float y, float z) {
  v->x += x;
  v->y += y;
  v->z += z;
  return v;
}

float ct_dot3fv(CT_Vec3f *a, CT_Vec3f *b) {
  return a->x * b->x + a->y * b->y + a->z * b->z;
}

CT_Vec3f *ct_cross3fv(CT_Vec3f *a, CT_Vec3f *b, CT_MPool* mpool) {
  CT_Vec3f *v = ALLOCATE_TYPE(mpool, CT_Vec3f);
  if (v != NULL) {
    v->x = a->y * b->z - a->z * b->y;
    v->y = a->z * b->x - a->x * b->z;
    v->z = a->x * b->y - a->y * b->x;
  }
  return v;
}
