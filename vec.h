#pragma once

#include "mpool.h"

typedef struct {
  float x,y;
} CT_Vec2f;

typedef struct {
  float x,y,z;
} CT_Vec3f;

CT_Vec2f *ct_vec2f(float x, float y, CT_MPool* mpool);
CT_Vec2f *ct_set2fv(CT_Vec2f *a, CT_Vec2f *b);
CT_Vec2f *ct_set2fxy(CT_Vec2f *v, float x, float y);
CT_Vec2f *ct_add2fv(CT_Vec2f *a, CT_Vec2f *b, CT_MPool* mpool);
CT_Vec2f *ct_add2fv_imm(CT_Vec2f *a, CT_Vec2f *b);
CT_Vec2f *ct_add2fn(CT_Vec2f *v, float n, CT_MPool* mpool);
CT_Vec2f *ct_add2fn_imm(CT_Vec2f *v, float n);
CT_Vec2f *ct_add2fxy(CT_Vec2f *v, float x, float y, CT_MPool* mpool);
CT_Vec2f *ct_add2fxy_imm(CT_Vec2f *v, float x, float y);

float ct_dot2fv(CT_Vec2f *a, CT_Vec2f *b);

CT_Vec3f *ct_vec3f(float x, float y, float z, CT_MPool* mpool);
CT_Vec3f *ct_set3fv(CT_Vec3f *a, CT_Vec3f *b);
CT_Vec3f *ct_set3fxyz(CT_Vec3f *a, float x, float y, float z);
CT_Vec3f *ct_add3fv(CT_Vec3f *a, CT_Vec3f *b, CT_MPool* mpool);
CT_Vec3f *ct_add3fv_imm(CT_Vec3f *a, CT_Vec3f *b);
CT_Vec3f *ct_add3fn(CT_Vec3f *a, float n, CT_MPool* mpool);
CT_Vec3f *ct_add3fn_imm(CT_Vec3f *a, float n);
CT_Vec3f *ct_add3fxyz(CT_Vec2f *v, float x, float y, float z, CT_MPool* mpool);
CT_Vec3f *ct_add3fxyz_imm(CT_Vec2f *v, float x, float y, float z);

float ct_dot3fv(CT_Vec3f *a, CT_Vec3f *b);
CT_Vec3f *ct_cross3fv(CT_Vec3f *a, CT_Vec3f *b, CT_MPool* mpool);
