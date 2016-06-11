#include "math/poisson.h"
#include "common/dbg.h"
#include "common/test.h"

CT_TEST_DECLS

int test_poisson() {
  CT_Quadtree t;
  CT_MPool vpool;
  int w = 600, h = 600;
  CT_IS(!ct_qtree_init(&t, 0, 0, w, h, 0x1000), "init");
  CT_IS(!ct_mpool_init(&vpool, 0x1000, sizeof(CT_Vec2f)), "init pool");
  srand(0);
  float r       = 10;
  size_t count  = 0;
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
  ct_mpool_free_all(&vpool);
  ct_qtree_free(&t);
  return 0;
}
