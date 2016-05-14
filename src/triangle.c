#include "triangle.h"

CT_EXPORT CT_Triangle2f *ct_triangle2f_init(CT_Triangle2f *t, CT_Vec2f *a,
                                            CT_Vec2f *b, CT_Vec2f *c) {
  t->a = *a;
  t->b = *b;
  t->c = *c;
  return t;
}

CT_EXPORT CT_Triangle2f *ct_triangle2f_initp(CT_Triangle2f *t,
                                             CT_Vec2f *points) {
  t->a = *points++;
  t->b = *points++;
  t->c = *points;
  return t;
}

CT_EXPORT CT_Triangle2f *ct_triangle2f_initxy(CT_Triangle2f *t, float x1,
                                              float y1, float x2, float y2,
                                              float x3, float y3) {
  t->buf[0] = x1;
  t->buf[1] = y1;
  t->buf[2] = x2;
  t->buf[3] = y2;
  t->buf[4] = x3;
  t->buf[5] = y3;
  return t;
}
