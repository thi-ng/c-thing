#include "dbg.h"
#include "test.h"

#include "quadtree.h"

CT_TEST_DECLS

struct bounds_t {
  CT_Vec2f min, max;
  size_t num;
};

void ct_qtree_bounds(CT_QuadTree *q, void *state) {
  struct bounds_t *bounds = (struct bounds_t *)state;
  ct_min2fv_imm(&bounds->min, q->point);
  ct_max2fv_imm(&bounds->max, q->point);
  bounds->num++;
}

int test_quadtree() {
  CT_INFO("%zd", sizeof(CT_QuadTree));
  CT_QuadTree q;
  CT_MPool qpool, vpool;
  ct_qtree_init(&q, 0, 0, 100, 100);
  ct_mpool_init(&qpool, 0x10000, sizeof(CT_QuadTree));
  ct_mpool_init(&vpool, 0x10000, sizeof(CT_Vec2f));
  CT_Vec2f *a = ct_vec2f(10, 10, &vpool);
  CT_Vec2f *b = ct_vec2f(10, 11, &vpool);
  CT_Vec2f *b2 = ct_vec2f(10.1, 11, &vpool);
  ct_qtree_insert(&q, a, NULL, &qpool);
  ct_qtree_insert(&q, b, NULL, &qpool);
  CT_IS(ct_qtree_find_leaf(&q, a), "can't find a");
  CT_IS(ct_qtree_find_leaf(&q, b), "can't find b");
  CT_IS(!ct_qtree_find_leaf(&q, b2), "shouldn't find b2");
  //srand(time(0));
  for (int i = 0; i < 1e5; i++) {
    CT_Vec2f *p =
        ct_vec2f(ct_rand_normpos() * 100, ct_rand_normpos() * 100, &vpool);
    ct_qtree_insert(&q, p, NULL, &qpool);
  }
  struct bounds_t bounds = {{1000, 1000}, {-1000, -1000}, 0};
  ct_qtree_visit_leaves(&q, ct_qtree_bounds, &bounds);
  CT_IS(100002 == bounds.num, "wrong leaf count: %zd", bounds.num);
  CT_INFO("%f,%f -> %f, %f, %zd", bounds.min.x, bounds.min.y, bounds.max.x,
          bounds.max.y, bounds.num);
  //ct_qtree_trace(&q, 0);
  ct_mpool_trace(&qpool);
  ct_mpool_free_all(&vpool);
  ct_mpool_free_all(&qpool);
  return 0;
}
