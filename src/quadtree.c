#include "config.h"
#include "dbg.h"

#include "quadtree.h"

ct_inline size_t child_index(const CT_QuadTree *q, const CT_Vec2f *p) {
  return (p->x < q->cx ? (p->y < q->cy ? 0 : 2) : (p->y < q->cy ? 1 : 3));
}

ct_inline void clear_children(CT_QuadTree *q) {
  q->children[0] = q->children[1] = q->children[2] = q->children[3] = NULL;
}

static int path_for_point(CT_QuadTree *q, CT_Vec2f *p, CT_QuadTree **path) {
  size_t i = 0;
  *path++ = q;
  while (q->type == CT_QT_BRANCH) {
    q = q->children[child_index(q, p)];
    if (q == NULL) {
      break;
    }
    *path++ = q;
    i++;
  }
  return (q->type == CT_QT_LEAF && ct_deltaeq2fv(q->point, p, EPS)) ? i : -1;
}

static size_t make_leaf(CT_QuadTree *q, size_t idx, CT_Vec2f *p, void *data,
                        CT_MPool *pool) {
  CT_QuadTree *c = CT_MP_ALLOC(pool, CT_QuadTree);
  CT_CHECK_MEM(c);
  clear_children(c);
  c->x = q->coords[idx & 1];
  c->y = q->coords[(idx >> 1) + 2];
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

CT_EXPORT void ct_qtree_init(CT_QuadTree *q, float x, float y, float w,
                             float h) {
  clear_children(q);
  q->x = x;
  q->y = y;
  q->cx = x + w * 0.5f;
  q->cy = y + h * 0.5f;
  q->type = CT_QT_EMPTY;
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
    case CT_QT_LEAF:
      if (ct_deltaeq2fv(q->point, p, EPS)) {
        q->point = p;
        q->data = data;
        return 0;
      } else {
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

size_t ct_qtree_remove(CT_QuadTree *q, CT_Vec2f *p, CT_MPool *pool) {
  CT_QuadTree *path[16];
  int d = path_for_point(q, p, path);
  CT_INFO("path depth: %d", d);
  switch (d) {
    case -1:
      return 1;
    case 0:
      q->point = q->data = NULL;
      q->type = CT_QT_EMPTY;
      break;
    default:
      while (d > 0) {
        CT_MP_FREE(pool, path[d]);
        d--;
        q = path[d];
        q->children[child_index(q, p)] = NULL;
        if (q->children[0] != NULL || q->children[1] != NULL ||
            q->children[2] != NULL || q->children[3] != NULL) {
          break;
        }
        q->type = CT_QT_EMPTY;
      }
  }
  return 0;
}

CT_EXPORT CT_QuadTree *ct_qtree_find_leaf(CT_QuadTree *q, CT_Vec2f *p) {
  CT_QuadTree *c;
  size_t idx;
  while (q->type == CT_QT_BRANCH) {
    idx = child_index(q, p);
    c = q->children[idx];
    if (c == NULL) {
      return NULL;
    }
    q = c;
  }
  return ((q->type == CT_QT_LEAF && ct_deltaeq2fv(q->point, p, EPS)) ? q
                                                                     : NULL);
}

CT_EXPORT void ct_qtree_trace_node(CT_QuadTree *q, size_t depth) {
  if (q->point) {
    CT_INFO("d: %zd: b: [%f,%f,%f,%f] c: [%p,%p,%p,%p] t: %zu, p: (%f,%f)",
            depth, q->x, q->y, q->cx, q->cy, q->children[0], q->children[1],
            q->children[2], q->children[3], q->type, q->point->x, q->point->y);
  } else {
    CT_INFO("d: %zd: b: [%f,%f,%f,%f] c: [%p,%p,%p,%p] t: %zu", depth, q->x,
            q->y, q->cx, q->cy, q->children[0], q->children[1], q->children[2],
            q->children[3], q->type);
  }
}

CT_EXPORT void ct_qtree_trace(CT_QuadTree *q, size_t depth) {
  if (q != NULL) {
    ct_qtree_trace_node(q, depth);
    for (size_t i = 0; i < 4; i++) {
      ct_qtree_trace(q->children[i], depth + 1);
    }
  }
}

CT_EXPORT void ct_qtree_visit_leaves(CT_QuadTree *q, CT_QuadTreeVisitor visit,
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

int main__() {
  CT_INFO("size: %zd", sizeof(CT_QuadTree));
  CT_MPool mp;
  ct_mpool_init(&mp, 4, sizeof(CT_QuadTree));
  CT_QuadTree q;
  ct_qtree_init(&q, 0, 0, 100, 100);
  CT_Vec2f *a = ct_vec2f(10, 10, NULL);
  CT_Vec2f *b = ct_vec2f(10, 11, NULL);
  CT_Vec2f *c = ct_vec2f(50, 12, NULL);
  ct_qtree_insert(&q, a, NULL, &mp);
  ct_qtree_insert(&q, b, NULL, &mp);
  ct_qtree_insert(&q, c, NULL, &mp);
  ct_qtree_trace(&q, 0);

  CT_INFO("---a removed");
  ct_qtree_remove(&q, a, &mp);
  ct_qtree_trace(&q, 0);

  CT_INFO("---b removed");
  ct_qtree_remove(&q, b, &mp);
  ct_qtree_trace(&q, 0);

  CT_INFO("---c removed");
  ct_qtree_remove(&q, c, &mp);
  ct_qtree_trace(&q, 0);

  CT_INFO("---c add");
  ct_qtree_insert(&q, c, NULL, &mp);
  ct_qtree_insert(&q, a, NULL, &mp);
  ct_qtree_trace(&q, 0);
  free(a);
  free(b);
  free(c);
  ct_mpool_trace(&mp);
  ct_mpool_free_all(&mp);
  return 0;
}

int main_() {
  CT_QuadTree q;
  ct_qtree_init(&q, 0, 0, 100, 90);
  CT_Vec2f *a = ct_vec2f(20, 50, NULL);
  size_t idx = child_index(&q, a);
  CT_INFO("%zd, %zd, %zd", idx, idx & 1, ((idx >> 1) & 1) + 2);
  CT_INFO("%zd, %f, %f", idx, q.coords[idx & 1],
          q.coords[((idx >> 1) & 1) + 2]);
  CT_INFO("%f, %f, %f, %f", q.coords[0], q.coords[1], q.coords[2], q.coords[3]);
  ct_qtree_trace(&q, 0);
  free(a);
  return 0;
}
