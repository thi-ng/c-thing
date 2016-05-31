#include "config.h"
#include "dbg.h"

#include "quadtree.h"

ct_inline size_t child_index(const CT_Quadtree *q, const CT_Vec2f *p) {
  return (p->y < q->cy ? 0 : 2) + (p->x < q->cx ? 0 : 1);
}

ct_inline void clear_children(CT_Quadtree *q) {
  q->children[0] = q->children[1] = q->children[2] = q->children[3] = NULL;
}

static int path_for_point(CT_Quadtree *q, CT_Vec2f *p, CT_Quadtree **path) {
  size_t i = 0;
  *path++ = q;
  while (q->type == CT_TREE_BRANCH) {
    q = q->children[child_index(q, p)];
    if (q == NULL) {
      return -1;
    }
    *path++ = q;
    i++;
  }
  return (q->type == CT_TREE_LEAF && ct_deltaeq2fv(q->point, p, EPS)) ? i : -1;
}

static size_t make_leaf(CT_Quadtree *q, size_t idx, CT_Vec2f *p, void *data,
                        CT_MPool *pool) {
  CT_Quadtree *c = CT_MP_ALLOC(pool, CT_Quadtree);
  CT_CHECK_MEM(c);
  clear_children(c);
  c->x = q->coords[idx & 1];
  c->y = q->coords[(idx >> 1) + 2];
  c->w = q->w * 0.5f;
  c->h = q->h * 0.5f;
  c->cx = c->x + c->w;
  c->cy = c->y + c->h;
  c->type = CT_TREE_LEAF;
  c->point = p;
  c->data = data;
  q->children[idx] = c;
  q->type = CT_TREE_BRANCH;
  return 0;
fail:
  return 1;
}

CT_EXPORT void ct_qtree_init(CT_Quadtree *q, float x, float y, float w,
                             float h) {
  clear_children(q);
  q->x = x;
  q->y = y;
  q->w = w;
  q->h = h;
  q->cx = x + w * 0.5f;
  q->cy = y + h * 0.5f;
  q->type = CT_TREE_EMPTY;
}

CT_EXPORT size_t ct_qtree_insert(CT_Quadtree *q, CT_Vec2f *p, void *data,
                                 CT_MPool *pool) {
  CT_CHECK(q != NULL, "tree is NULL");
  CT_CHECK(p != NULL, "point is NULL");
  CT_Quadtree *c;
  size_t idx;
  while (q->type == CT_TREE_BRANCH) {
    idx = child_index(q, p);
    c = q->children[idx];
    if (c == NULL) break;
    q = c;
  }
  switch (q->type) {
    case CT_TREE_EMPTY:
      q->point = p;
      q->data = data;
      q->type = CT_TREE_LEAF;
      return 0;
    case CT_TREE_LEAF:
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
    case CT_TREE_BRANCH:
      return make_leaf(q, idx, p, data, pool);
    default:
      CT_SENTINEL("invalid node type: %zu", q->type);
  }
fail:
  return 1;
}

size_t ct_qtree_remove(CT_Quadtree *q, CT_Vec2f *p, CT_MPool *pool) {
  CT_Quadtree *path[24];
  int d = path_for_point(q, p, path);
  switch (d) {
    case -1:
      return 1;
    case 0:
      q->point = q->data = NULL;
      q->type = CT_TREE_EMPTY;
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
        q->type = CT_TREE_EMPTY;
      }
  }
  return 0;
}

CT_EXPORT CT_Quadtree *ct_qtree_find_leaf(CT_Quadtree *q, CT_Vec2f *p) {
  CT_Quadtree *c;
  size_t idx;
  while (q->type == CT_TREE_BRANCH) {
    idx = child_index(q, p);
    c = q->children[idx];
    if (c == NULL) {
      return NULL;
    }
    q = c;
  }
  return ((q->type == CT_TREE_LEAF && ct_deltaeq2fv(q->point, p, EPS)) ? q
                                                                       : NULL);
}

CT_EXPORT void ct_qtree_trace_node(CT_Quadtree *q, size_t depth) {
  if (q->point) {
    CT_INFO("d: %zd: %p b: [%f,%f,%f,%f] c: [%p,%p,%p,%p] t: %zu, p: (%f,%f)",
            depth, q, q->x, q->y, q->w, q->h, q->children[0], q->children[1],
            q->children[2], q->children[3], q->type, q->point->x, q->point->y);
  } else {
    CT_INFO("d: %zd: %p b: [%f,%f,%f,%f] c: [%p,%p,%p,%p] t: %zu", depth, q,
            q->x, q->y, q->w, q->h, q->children[0], q->children[1],
            q->children[2], q->children[3], q->type);
  }
}

CT_EXPORT void ct_qtree_trace(CT_Quadtree *q, size_t depth) {
  if (q != NULL) {
    ct_qtree_trace_node(q, depth);
    for (size_t i = 0; i < 4; i++) {
      ct_qtree_trace(q->children[i], depth + 1);
    }
  }
}

CT_EXPORT int ct_qtree_visit_leaves(CT_Quadtree *q, CT_QuadtreeVisitor visit,
                                    void *state) {
  switch (q->type) {
    case CT_TREE_LEAF:
      return visit(q, state);
    case CT_TREE_BRANCH:
      for (size_t i = 0; i < 4; i++) {
        if (q->children[i]) {
          int res = ct_qtree_visit_leaves(q->children[i], visit, state);
          if (res) {
            return res;
          }
        }
      }
  }
  return 0;
}

CT_EXPORT int ct_qtree_visit(CT_Quadtree *q, CT_QuadtreeVisitor visit,
                             void *state) {
  int res = visit(q, state);
  if (res) {
    return res;
  }
  if (q->type == CT_TREE_BRANCH) {
    for (size_t i = 0; i < 4; i++) {
      if (q->children[i]) {
        ct_qtree_visit(q->children[i], visit, state);
      }
    }
  }
  return 0;
}
