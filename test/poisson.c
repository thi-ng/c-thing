#include <stdio.h>

#include "common/dbg.h"
#include "common/test.h"
#include "math/poisson.h"

CT_TEST_DECLS

int test_poisson() {
  CT_Quadtree t;
  CT_MPool vpool;
  int w = 600, h = 600;
  CT_IS(!ct_qtree_init(&t, 0, 0, w, h, 0x10000), "init");
  CT_IS(!ct_mpool_init(&vpool, 0x1000, sizeof(CT_Vec2f)), "init pool");
  FILE *f = fopen("poisson.svg", "w");
  fprintf(f,
          "<?xml version=\"1.0\"?>\n<svg version=\"1.1\" width=\"%u\" "
          "height=\"%u\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" "
          "xmlns=\"http://www.w3.org/2000/svg\">\n",
          w, h);
  fprintf(f, "<g stroke=\"none\" fill=\"black\">\n");
  srand(time(0));
  float r      = 5;
  size_t count = 0;
  CT_Vec2f *p  = (CT_Vec2f *)ct_mpool_alloc(&vpool);
  for (size_t i = 0; i < 10000; i++) {
    if (ct_poisson_sample2f(&t, r, 20, p)) {
      fprintf(f, "<circle cx=\"%f\" cy=\"%f\" r=\"%f\"/>", p->x, p->y, 2.f);
      p = (CT_Vec2f *)ct_mpool_alloc(&vpool);
      count++;
    }
  }
  fprintf(f, "</g></svg>");
  fclose(f);
  CT_INFO("%zu points", count);
  ct_mpool_free_all(&vpool);
  ct_qtree_free(&t);
  return 0;
}
