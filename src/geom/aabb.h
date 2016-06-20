#pragma once

#include "cthing.h"

#include "math/vec.h"
#include "mem/mpool.h"

CT_BEGIN_DECLS

typedef struct {
  CT_Vec3f p;
  CT_Vec3f size;
} CT_AABB;

CT_AABB *ct_aabbv(CT_Vec3f *p, CT_Vec3f *size, CT_MPool *mpool);
CT_AABB *ct_aabbn(CT_Vec3f *p, float size, CT_MPool *mpool);

float ct_aabb_area(void *a);
int ct_aabb_classify_point(void *a, const CT_Vec3f *p);

CT_END_DECLS
