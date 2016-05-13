#pragma once

#include "mpool.h"
#include "vec.h"

typedef struct {
  CT_Vec2f p;
  CT_Vec2f size;
} CT_Rect2;

CT_Rect2 *ct_rect2v(CT_Vec2f *p, CT_Vec2f *size, CT_MPool *mpool);
CT_Rect2 *ct_rect2n(CT_Vec2f *p, float size, CT_MPool *mpool);

float ct_rect_area(void *a);
int8_t ct_rect_classify_point(void *a, CT_Vec2f *p);
