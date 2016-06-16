#include "geom/clip.h"
#include "common/test.h"

CT_TEST_DECLS

static int clip(CT_Vec2f *p, CT_Vec2f *q, float c[static 4]) {
  CT_Vec2f a, b, ca, cb;
  ct_set2fp(&a, c);
  ct_set2fp(&b, c + 2);
  if (ct_clip2f_liangbarsky(p, q, &a, &b, &ca, &cb)) {
    return ct_deltaeq2fv(&ca, (CT_Vec2f *)&c[4], 0.1) &&
           ct_deltaeq2fv(&cb, (CT_Vec2f *)&c[6], 0.1);
  }
  return -1;
}

int test_clipping() {
  CT_Vec2f p = {-100, -100};
  CT_Vec2f q = {100, 100};
  CT_IS(1 == clip(&p, &q, (float[]){0, -99, 0, 99, 0, -99, 0, 99}),
        "inside vert");
  CT_IS(1 == clip(&p, &q, (float[]){-99, 0, 99, 0, -99, 0, 99, 0}),
        "inside horz");
  CT_IS(1 == clip(&p, &q, (float[]){0, 0, 0, 0, 0, 0, 0, 0}), "inside p");
  CT_IS(-1 == clip(&p, &q, (float[]){-101, -51, -101, 101}), "left");
  CT_IS(-1 == clip(&p, &q, (float[]){101, -49, 101, 99}), "right");
  CT_IS(-1 == clip(&p, &q, (float[]){-200, -101, 0, -101}), "top");
  CT_IS(-1 == clip(&p, &q, (float[]){0, 101, 200, 101}), "bottom");
  CT_IS(1 == clip(&p, &q,
                  (float[]){-110, 100, 110, -100, -100, 90.9, 100, -90.9}),
        "bl tr");
  CT_IS(1 == clip(&p, &q,
                  (float[]){-100, 110, 100, -110, -90.9, 100, 90.9, -100}),
        "bl tr 2");
  return 0;
}
