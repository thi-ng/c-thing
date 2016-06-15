#include "data/quadtree.h"
#include "common/dbg.h"
#include "common/test.h"
#include "geom/isec.h"

CT_TEST_DECLS

struct bounds_t {
  CT_Vec2f min, max;
  size_t num;
};

struct isec_t {
  CT_Vec2f c;
  float r;
  CT_Vec2f *sel[4];
  size_t num;
};

static int ct_qtree_bounds(const CT_QTNode *node, void *state) {
  struct bounds_t *bounds = (struct bounds_t *)state;
  ct_min2fv_imm(&bounds->min, node->point);
  ct_max2fv_imm(&bounds->max, node->point);
  bounds->num++;
  return 0;
}

static int ct_qtree_select(const CT_QTNode *node, void *state) {
  struct isec_t *isec = (struct isec_t *)state;
  CT_Vec2f p          = {node->x + node->w, node->y + node->h};
  int i = ct_intersect_rect_circle(&node->pos, &p, &isec->c, isec->r);
  //CT_INFO("isec: %d, n: %1.3f,%1.3f -> %1.3f,%1.3f c: %f,%f, r: %f", i, node->x, node->y, p.x, p.y, isec->c.x, isec->c.y, isec->r);
  if (i) {
    if (node->type == CT_TREE_LEAF) {
      if (ct_dist2fv(node->point, &isec->c) <= isec->r) {
        isec->sel[isec->num++] = node->point;
      }
    }
    return 0;
  }
  return 1;
}

static void print_isec_results(struct isec_t *isec) {
  for (size_t i = 0; i < isec->num; i++) {
    CT_INFO("isec %zd: (%f, %f)", i, isec->sel[i]->x, isec->sel[i]->y);
  }
}

int test_quadtree() {
  CT_INFO("CT_Quadtree size: %zd", sizeof(CT_Quadtree));
  CT_INFO("CT_QTNode size: %zd", sizeof(CT_QTNode));
  CT_Quadtree t;
  CT_MPool vpool;
  ct_qtree_init(&t, 0, 0, 100, 100, 0x10000);
  ct_mpool_init(&vpool, 0x10000, sizeof(CT_Vec2f));
  CT_Vec2f *a  = ct_vec2f(10, 10, &vpool);
  CT_Vec2f *b  = ct_vec2f(10, 11, &vpool);
  CT_Vec2f *b2 = ct_vec2f(10.1, 11, &vpool);
  CT_Vec2f *c  = ct_vec2f(50, 12, &vpool);
  CT_IS(!ct_qtree_insert(&t, a, NULL), "add a");
  CT_IS(!ct_qtree_insert(&t, b, NULL), "add b");
  CT_IS(!ct_qtree_insert(&t, c, NULL), "add c");
  //ct_qtree_trace(&t);
  struct isec_t isec = {{50, 50}, 70.f, {NULL, NULL, NULL, NULL}, 0};
  ct_qtree_visit(&t, ct_qtree_select, &isec);
  CT_IS(3 == isec.num, "wrong isec count: %zd", isec.num);
  //print_isec_results(&isec);
  struct isec_t isec2 = {{10, 10}, 0.9f, {NULL, NULL, NULL, NULL}, 0};
  ct_qtree_visit(&t, ct_qtree_select, &isec2);
  CT_IS(1 == isec2.num, "wrong isec2 count: %zd", isec2.num);
  struct isec_t isec3 = {{52, 12}, 5.f, {NULL, NULL, NULL, NULL}, 0};
  ct_qtree_visit(&t, ct_qtree_select, &isec3);
  CT_IS(1 == isec3.num, "wrong isec3 count: %zd", isec3.num);
  CT_IS(ct_qtree_find_leaf(&t, a), "can't find a");
  CT_IS(ct_qtree_find_leaf(&t, b), "can't find b");
  CT_IS(!ct_qtree_find_leaf(&t, b2), "shouldn't find b2");
  ct_qtree_remove(&t, a);
  CT_IS(!ct_qtree_find_leaf(&t, a), "shouldn't find a");
  CT_IS(ct_qtree_remove(&t, a), "remove a again");
  ct_qtree_remove(&t, b);
  CT_IS(!ct_qtree_find_leaf(&t, b), "shouldn't find b");
  CT_IS(ct_qtree_remove(&t, b), "remove b again");
  ct_qtree_remove(&t, c);
  CT_IS(!ct_qtree_find_leaf(&t, c), "shouldn't find c");
  CT_IS(ct_qtree_remove(&t, c), "remove c again");
  CT_IS(CT_TREE_EMPTY == t.root.type, "root is not empty: %zd", t.root.type);
  //srand(time(0));
  int num = 1e5;
  for (int i = 0; i < num; i++) {
    CT_Vec2f *p =
        ct_vec2f(ct_rand_normpos() * 100, ct_rand_normpos() * 100, &vpool);
    ct_qtree_insert(&t, p, NULL);
  }
  struct bounds_t bounds = {{1000, 1000}, {-1000, -1000}, 0};
  ct_qtree_visit_leaves(&t, ct_qtree_bounds, &bounds);
  CT_IS(num == bounds.num, "wrong leaf count: %zd", bounds.num);
  CT_INFO("%f,%f -> %f, %f, %zd", bounds.min.x, bounds.min.y, bounds.max.x,
          bounds.max.y, bounds.num);
  ct_mpool_free_all(&vpool);
  //ct_mpool_trace(&t.pool);
  ct_qtree_free(&t);
  return 0;
}
