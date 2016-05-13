#include <assert.h>
#include <stdio.h>

#include "circle.h"

//extern CT_IArea __ct_iarea_circle2;
//extern CT_ICircumference __ct_icircumference_circle2;

void test_circle() {
  printf("---- test_circle ----\n");
  CT_MPool pool;
  ct_mpool_init(&pool, 16, sizeof(CT_Vec2f));
  CT_Circle2f *c = ct_circle2fr(1, NULL);
  CT_Circle2f *c2 = ct_circle2fr(2, NULL);
  assert(PI == ct_circle2f_area(c));
  assert(PI * 4 == ct_circle2f_area(c2));
  //assert(PI == ct_area(&__ct_iarea_circle2, c));
  assert(TAU == ct_circle2f_circumference(c));
  assert(TAU * 2 == ct_circle2f_circumference(c2));
  //assert(TAU == ct_circumference(&__ct_icircumference_circle2, c));
  CT_Vec2f *p = ct_vec2f(0, 0, &pool);
  assert(1 == ct_circle2f_classify_point2f(c, p));
  assert(0 == ct_circle2f_classify_point2f(c, ct_set2fxy(p, 1, 0)));
  assert(-1 == ct_circle2f_classify_point2f(c, ct_set2fxy(p, 1.001f, 0)));
  free(c);
  ct_mpool_free_all(&pool);
}
