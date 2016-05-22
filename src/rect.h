#pragma once

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mpool.h"
#include "vec.h"

typedef struct {
  CT_Vec2f p;
  CT_Vec2f size;
} CT_Rect2;

CT_Rect2 *ct_rect2v(CT_Vec2f *p, CT_Vec2f *size, CT_MPool *mpool);
CT_Rect2 *ct_rect2n(CT_Vec2f *p, float size, CT_MPool *mpool);

float ct_rect_area(void *a);
int ct_rect_classify_point(void *a, CT_Vec2f *p);

#ifdef __cplusplus
}
#endif
