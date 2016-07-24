#include "test/test.h"

#include "geom/circle.h"

//extern CT_IArea __ct_iarea_circle2;
//extern CT_ICircumference __ct_icircumference_circle2;

CT_TEST_DECLS

int test_circle() {
  CT_Circle2f c, c2;
  ct_circle2f_initr(&c, 1);
  ct_circle2f_initr(&c2, 2);
  CT_IS(CT_PI == ct_circle2f_area(&c), "area c");
  CT_IS(CT_PI * 4 == ct_circle2f_area(&c2), "area c2");
  CT_IS(CT_TAU == ct_circle2f_circumference(&c), "circum c");
  CT_IS(CT_TAU * 2 == ct_circle2f_circumference(&c2), "circum c2");
  CT_Vec2f p = {.x = 0, .y = 0};
  CT_IS(1 == ct_circle2f_classify_point(&c, &p), "classify 1");
  CT_IS(0 == ct_circle2f_classify_point(&c, ct_set2fxy(&p, 1, 0)),
        "classify 0");
  CT_IS(-1 == ct_circle2f_classify_point(&c, ct_set2fxy(&p, 1.001f, 0)),
        "classify -1");
  return 0;
}
