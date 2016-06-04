#pragma once

#include "config.h"
#include "math/vec.h"
#include "mem/mpool.h"

CT_BEGIN_DECLS

typedef union {
  struct {
    CT_Vec2f pos;
    CT_Vec2f size;
  };
  float buf[4];
} CT_Rect2f;

CT_Rect2f *ct_rect2fv(CT_Vec2f *p, CT_Vec2f *size, CT_MPool *mpool);
CT_Rect2f *ct_rect2fn(CT_Vec2f *p, float size, CT_MPool *mpool);

float ct_rect2f_area(void *a);
int ct_rect2f_classify_point(void *a, CT_Vec2f *p);

CT_END_DECLS
