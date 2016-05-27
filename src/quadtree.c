#include <stdio.h>
#include <stdlib.h>

#include "dbg.h"
#include "test.h"
#include "vec.h"

typedef struct CT_QuadTree CT_QuadTree;

enum { CT_QT_EMPTY = 0, CT_QT_BRANCH, CT_QT_LEAF };

struct CT_QuadTree {
  CT_QuadTree *children[4];
  CT_Vec2f *point;
  void *data;
  float x, y, cx, cy;
  size_t type;
};

struct bounds_t {
  CT_Vec2f min, max;
};

typedef void (*CT_QuadTreeVisitor)(CT_QuadTree *, void *);

CT_TEST_DECLS

void ct_qtree_trace_node(CT_QuadTree *q, size_t d);

ct_inline size_t child_index(const CT_QuadTree *q, const CT_Vec2f *p) {
  return (p->x < q->cx ? (p->y < q->cy ? 0 : 2) : (p->y < q->cy ? 1 : 3));
}

static size_t make_leaf(CT_QuadTree *q, size_t idx, CT_Vec2f *p, void *data,
                        CT_MPool *pool) {
  CT_QuadTree *c = CT_MP_ALLOC_STD(pool, CT_QuadTree);
  CT_CHECK_MEM(c);
  c->children[0] = c->children[1] = c->children[2] = c->children[3] = NULL;
  c->x = (idx & 1) ? q->cx : q->x;
  c->y = (idx & 2) ? q->cy : q->y;
  c->cx = c->x + (q->cx - q->x) * 0.5f;
  c->cy = c->y + (q->cy - q->y) * 0.5f;
  c->type = CT_QT_LEAF;
  c->point = p;
  c->data = data;
  q->children[idx] = c;
  q->type = CT_QT_BRANCH;
  return 0;
fail:
  return 1;
}

CT_EXPORT size_t ct_qtree_insert(CT_QuadTree *q, CT_Vec2f *p, void *data,
                                 CT_MPool *pool) {
  CT_CHECK(q != NULL, "tree is NULL");
  CT_CHECK(p != NULL, "point is NULL");
  CT_QuadTree *c;
  size_t idx;
  while (q->type == CT_QT_BRANCH) {
    idx = child_index(q, p);
    c = q->children[idx];
    if (c == NULL) break;
    q = c;
  }
  switch (q->type) {
    case CT_QT_EMPTY:
      q->point = p;
      q->data = data;
      q->type = CT_QT_LEAF;
      return 0;
    case CT_QT_LEAF: {
      CT_Vec2f *op = q->point;
      void *od = q->data;
      q->point = q->data = NULL;
      if (!make_leaf(q, child_index(q, p), p, data, pool)) {
        return ct_qtree_insert(q, op, od, pool);
      }
      return 1;
    }
    case CT_QT_BRANCH:
      return make_leaf(q, idx, p, data, pool);
    default:
      CT_SENTINEL("invalid node type: %zu", q->type);
  }
fail:
  return 1;
}

void ct_qtree_trace_node(CT_QuadTree *q, size_t depth) {
  if (q->point) {
    CT_INFO("d: %zd: b: [%f,%f,%f,%f] c: [%p,%p,%p,%p] t: %zu, p: (%f,%f)\n",
            depth, q->x, q->y, q->cx, q->cy, q->children[0], q->children[1],
            q->children[2], q->children[3], q->type, q->point->x, q->point->y);
  } else {
    CT_INFO("d: %zd: b: [%f,%f,%f,%f] c: [%p,%p,%p,%p] t: %zu\n", depth, q->x,
            q->y, q->cx, q->cy, q->children[0], q->children[1], q->children[2],
            q->children[3], q->type);
  }
}

void ct_qtree_trace(CT_QuadTree *q, size_t depth) {
  if (q != NULL) {
    ct_qtree_trace_node(q, depth);
    for (size_t i = 0; i < 4; i++) {
      ct_qtree_trace(q->children[i], depth + 1);
    }
  }
}

void ct_qtree_visit_leaves(CT_QuadTree *q, CT_QuadTreeVisitor visit,
                           void *state) {
  switch (q->type) {
    case CT_QT_LEAF:
      visit(q, state);
      break;
    case CT_QT_BRANCH:
      for (size_t i = 0; i < 4; i++) {
        if (q->children[i]) {
          ct_qtree_visit_leaves(q->children[i], visit, state);
        }
      }
  }
}

void ct_qtree_bounds(CT_QuadTree *q, void *state) {
  struct bounds_t *bounds = (struct bounds_t *)state;
  ct_min2fv_imm(&bounds->min, q->point);
  ct_max2fv_imm(&bounds->max, q->point);
}

int qt_bench() {
  CT_INFO("%zd", sizeof(CT_QuadTree));
  CT_QuadTree q = {.children = {NULL, NULL, NULL, NULL},
                   .x = 0,
                   .y = 0,
                   .cx = 50,
                   .cy = 50,
                   .type = CT_QT_EMPTY};
  CT_MPool qpool, vpool;
  ct_mpool_init(&qpool, 2e5, sizeof(CT_QuadTree));
  ct_mpool_init(&vpool, 1e5, sizeof(CT_Vec2f));
  //srand(time(0));
  for (int i = 0; i < 1e5; i++) {
    CT_Vec2f *p =
        ct_vec2f(ct_rand_normpos() * 100, ct_rand_normpos() * 100, &vpool);
    ct_qtree_insert(&q, p, NULL, &qpool);
  }
  struct bounds_t bounds = {{1000, 1000}, {-1000, -1000}};
  ct_qtree_visit_leaves(&q, ct_qtree_bounds, &bounds);
  CT_INFO("%f,%f -> %f, %f", bounds.min.x, bounds.min.y, bounds.max.x,
          bounds.max.y);
  //ct_qtree_trace(&q, 0);
  ct_mpool_trace(&qpool);
  ct_mpool_free_all(&vpool);
  ct_mpool_free_all(&qpool);
  return 0;
}

int main() {
  CT_RUN_TEST(qt_bench);
}
