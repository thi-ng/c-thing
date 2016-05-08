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

VEC2OP(CT_Vec2f, float, ct_add2f, ct_set2f, +)

VEC2OP(CT_Vec2f, float, ct_sub2f, ct_set2f, -)

VEC2OP(CT_Vec2f, float, ct_mul2f, ct_set2f, *)

VEC2OP(CT_Vec2f, float, ct_div2f, ct_set2f, /)

float ct_dot2fv(CT_Vec2f *a, CT_Vec2f *b) {
  return a->x * b->x + a->y * b->y;
}

CT_Vec2f *ct_normalize2f_imm(CT_Vec2f *v, float len) {
  float m = v->x * v->x + v->y * v->y;
  if (m > 0.0) {
    len /= m;
    v->x *= len;
    v->y *= len;
  }
  return v;
}

CT_Vec2f *ct_normalize2f(CT_Vec2f *v, float len, CT_MPool *mpool) {
  return ct_normalize2f_imm(ct_set2fv(ALLOCATE_TYPE(mpool, CT_Vec2f), v), len);
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

CT_Vec3f *ct_add3fv_imm(CT_Vec3f *a, CT_Vec3f *b) {
  a->x += b->x;
  a->y += b->y;
  a->z += b->z;
  return a;
}

CT_Vec3f *ct_add3fv(CT_Vec3f *a, CT_Vec3f *b, CT_MPool* mpool) {
  return ct_add3fv_imm(ct_set3fv(ALLOCATE_TYPE(mpool, CT_Vec3f), a), b);
}

CT_Vec3f *ct_add3fn_imm(CT_Vec3f *v, float n) {
  v->x += n;
  v->y += n;
  v->z += n;
  return v;
}

CT_Vec3f *ct_add3fn(CT_Vec3f *a, float n, CT_MPool* mpool) {
  return ct_add3fn_imm(ct_set3fv(ALLOCATE_TYPE(mpool, CT_Vec3f), a), n);
}

CT_Vec3f *ct_add3fxyz_imm(CT_Vec3f *v, float x, float y, float z) {
  v->x += x;
  v->y += y;
  v->z += z;
  return v;
}

CT_Vec3f *ct_add3fxyz(CT_Vec3f *a, float x, float y, float z, CT_MPool* mpool) {
  return ct_add3fxyz_imm(ct_set3fv(ALLOCATE_TYPE(mpool, CT_Vec3f), a), x, y, z);
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

CT_Vec3f *ct_cross3fv(CT_Vec3f *a, CT_Vec3f *b, CT_MPool* mpool) {
  CT_Vec3f *v = ALLOCATE_TYPE(mpool, CT_Vec3f);
  v->x = a->y * b->z - a->z * b->y;
  v->y = a->z * b->x - a->x * b->z;
  v->z = a->x * b->y - a->y * b->x;
  return v;
}

CT_Vec3f *ct_normalize3f_imm(CT_Vec3f *v, float len) {
  float m = v->x * v->x + v->y * v->y + v->z * v->z;
  if (m > 0.0) {
    len /= m;
    v->x *= len;
    v->y *= len;
    v->z *= len;
  }
  return v;
}

CT_Vec3f *ct_normalize3f(CT_Vec3f *v, float len, CT_MPool *mpool) {
  return ct_normalize3f_imm(ct_set3fv(ALLOCATE_TYPE(mpool, CT_Vec3f), v), len);
}
