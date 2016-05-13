#pragma once

#include "mpool.h"
#include "vec.h"

typedef struct {
  CT_Vec3f p;
  CT_Vec3f size;
} CT_AABB;

CT_AABB *ct_aabbv(CT_Vec3f *p, CT_Vec3f *size, CT_MPool *mpool);
CT_AABB *ct_aabbn(CT_Vec3f *p, float size, CT_MPool *mpool);

float ct_aabb_area(void *a);
int8_t ct_aabb_classify_point(void *a, CT_Vec3f *p);
