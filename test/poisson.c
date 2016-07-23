#include "test/test.h"

#include "math/poisson.h"

CT_TEST_DECLS

int test_poisson() {
  srand(0);
  CT_DEF_MPOOL(vpool, 0x1000, CT_Vec2f);
  int w        = 600;
  int h        = 600;
  float r      = 10;
  size_t count = 0;

  CT_Quadtree t;
  CT_IS(!ct_qtree_init(&t, 0, 0, w, h, 0x1000), "init");

  size_t failed = 0;
  CT_Vec2f *p   = (CT_Vec2f *)ct_mpool_alloc(&vpool);

  while (1) {
    if (!ct_poisson_sample2f(&t, r, 20, p)) {
      p = (CT_Vec2f *)ct_mpool_alloc(&vpool);
      count++;
      failed = 0;
    } else if (++failed > 50) {
      break;
    }
  }
  CT_IS(2000 < count, "count: %zu", count);

  ct_mpool_free(&vpool);
  ct_qtree_free(&t);
  return 0;

fail:
  return 1;
}
