#include <assert.h>
#include <stdio.h>

#include "circle.h"

//extern CT_IArea __ct_iarea_circle2;
//extern CT_ICircumference __ct_icircumference_circle2;

void test_circle() {
  printf("---- test_circle ----\n");
  CT_Circle2f c, c2;
  ct_circle2f_initr(&c, 1);
  ct_circle2f_initr(&c2, 2);
  assert(PI == ct_circle2f_area(&c));
  assert(PI * 4 == ct_circle2f_area(&c2));
  //assert(PI == ct_area(&__ct_iarea_circle2, &c));
  assert(TAU == ct_circle2f_circumference(&c));
  assert(TAU * 2 == ct_circle2f_circumference(&c2));
  //assert(TAU == ct_circumference(&__ct_icircumference_circle2, &c));
  CT_Vec2f p = {.x = 0, .y = 0};
  assert(1 == ct_circle2f_classify_point(&c, &p));
  assert(0 == ct_circle2f_classify_point(&c, ct_set2fxy(&p, 1, 0)));
  assert(-1 == ct_circle2f_classify_point(&c, ct_set2fxy(&p, 1.001f, 0)));
}
