#include "triangle.h"

CT_EXPORT CT_Triangle2f *ct_triangle2fp(float *points, CT_MPool *mpool) {
  CT_Triangle2f *t = ALLOCATE_TYPE(mpool, CT_Triangle2f);
  if (t != NULL) {
    t->buf[0] = *points++;
    t->buf[1] = *points++;
    t->buf[2] = *points++;
    t->buf[3] = *points++;
    t->buf[4] = *points++;
    t->buf[5] = *points;
  }
  return t;
}

CT_EXPORT CT_Triangle2f *ct_triangle2fv(CT_Vec2f *a, CT_Vec2f *b, CT_Vec2f *c, CT_MPool *mpool) {
  CT_Triangle2f *t = ALLOCATE_TYPE(mpool, CT_Triangle2f);
  if (t != NULL) {
    t->a = *a;
    t->b = *b;
    t->c = *c;
  }
  return t;
}

CT_EXPORT CT_Triangle2f *ct_triangle2fxy(float x1, float y1, float x2, float y2, float x3, float y3, CT_MPool *mpool) {
  CT_Triangle2f *t = ALLOCATE_TYPE(mpool, CT_Triangle2f);
  if (t != NULL) {
    t->buf[0] = x1;
    t->buf[1] = y1;
    t->buf[2] = x2;
    t->buf[3] = y2;
    t->buf[4] = x3;
    t->buf[5] = y3;
  }
  return t;
}
