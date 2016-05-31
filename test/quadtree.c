#include "dbg.h"
#include "test.h"

#include "isec.h"
#include "quadtree.h"

CT_TEST_DECLS

struct bounds_t {
  CT_Vec2f min, max;
  size_t num;
};

struct isec_t {
  CT_Vec2f min, max, c;
  float r;
  CT_Vec2f *sel[4];
  size_t num;
};

int ct_qtree_bounds(CT_Quadtree *q, void *state) {
  struct bounds_t *bounds = (struct bounds_t *)state;
  ct_min2fv_imm(&bounds->min, q->point);
  ct_max2fv_imm(&bounds->max, q->point);
  bounds->num++;
  return 0;
}

int ct_qtree_select(CT_Quadtree *q, void *state) {
  //ct_qtree_trace_node(q, -1);
  struct isec_t *isec = (struct isec_t *)state;
  ct_set2fxy(&isec->min, q->x, q->y);
  ct_set2fxy(&isec->max, q->x + q->w, q->y + q->h);
  int i = ct_intersect_rect_circle(&isec->min, &isec->max, &isec->c, isec->r);
  //CT_INFO("isec: %d", i);
  if (i) {
    if (q->type == CT_TREE_LEAF) {
      isec->sel[isec->num++] = q->point;
    }
    return 0;
  }
  return 1;
}

void print_isec_results(struct isec_t *isec) {
  for (size_t i = 0; i < isec->num; i++) {
    CT_INFO("isec %zd: (%f, %f)", i, isec->sel[i]->x, isec->sel[i]->y);
  }
}

int test_quadtree() {
  CT_INFO("CT_Quadtree size: %zd", sizeof(CT_Quadtree));
  CT_Quadtree q;
  CT_MPool qpool, vpool;
  ct_qtree_init(&q, 0, 0, 100, 100);
  ct_mpool_init(&qpool, 0x10000, sizeof(CT_Quadtree));
  ct_mpool_init(&vpool, 0x10000, sizeof(CT_Vec2f));
  CT_Vec2f *a = ct_vec2f(10, 10, &vpool);
  CT_Vec2f *b = ct_vec2f(10, 11, &vpool);
  CT_Vec2f *b2 = ct_vec2f(10.1, 11, &vpool);
  CT_Vec2f *c = ct_vec2f(50, 12, &vpool);
  ct_qtree_insert(&q, a, NULL, &qpool);
  ct_qtree_insert(&q, b, NULL, &qpool);
  ct_qtree_insert(&q, c, NULL, &qpool);
  ct_qtree_trace(&q, 0);
  struct isec_t isec = {
      {0, 0}, {0, 0}, {50, 50}, 70.f, {NULL, NULL, NULL, NULL}, 0};
  ct_qtree_visit(&q, ct_qtree_select, &isec);
  CT_IS(3 == isec.num, "wrong isec count: %zd", isec.num);
  CT_IS(ct_qtree_find_leaf(&q, a), "can't find a");
  CT_IS(ct_qtree_find_leaf(&q, b), "can't find b");
  CT_IS(!ct_qtree_find_leaf(&q, b2), "shouldn't find b2");
  ct_qtree_remove(&q, a, &qpool);
  CT_IS(!ct_qtree_find_leaf(&q, a), "shouldn't find a");
  CT_IS(ct_qtree_remove(&q, a, &qpool), "remove a again");
  ct_qtree_remove(&q, b, &qpool);
  CT_IS(!ct_qtree_find_leaf(&q, b), "shouldn't find b");
  CT_IS(ct_qtree_remove(&q, b, &qpool), "remove b again");
  ct_qtree_remove(&q, c, &qpool);
  CT_IS(!ct_qtree_find_leaf(&q, c), "shouldn't find c");
  CT_IS(ct_qtree_remove(&q, c, &qpool), "remove c again");
  CT_IS(CT_TREE_EMPTY == q.type, "root is not empty: %zd", q.type);
  //srand(time(0));
  int num = 1e5;
  for (int i = 0; i < num; i++) {
    CT_Vec2f *p =
        ct_vec2f(ct_rand_normpos() * 100, ct_rand_normpos() * 100, &vpool);
    ct_qtree_insert(&q, p, NULL, &qpool);
  }
  struct bounds_t bounds = {{1000, 1000}, {-1000, -1000}, 0};
  ct_qtree_visit_leaves(&q, ct_qtree_bounds, &bounds);
  CT_IS(num == bounds.num, "wrong leaf count: %zd", bounds.num);
  CT_INFO("%f,%f -> %f, %f, %zd", bounds.min.x, bounds.min.y, bounds.max.x,
          bounds.max.y, bounds.num);
  //ct_qtree_trace(&q, 0);
  ct_mpool_trace(&qpool);
  ct_mpool_free_all(&vpool);
  ct_mpool_free_all(&qpool);
  return 0;
}
