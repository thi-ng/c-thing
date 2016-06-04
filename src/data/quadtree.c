#include "data/quadtree.h"
#include "common/dbg.h"
#include "config.h"

ct_inline size_t child_index(const CT_QTNode *q, const CT_Vec2f *p) {
  return (p->y < q->cy ? 0 : 2) + (p->x < q->cx ? 0 : 1);
}

ct_inline void clear_children(CT_QTNode *q) {
  q->children[0] = q->children[1] = q->children[2] = q->children[3] = NULL;
}

static int path_for_point(CT_QTNode *q, const CT_Vec2f *p, CT_QTNode **path) {
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

static size_t make_leaf(CT_QTNode *q, size_t idx, CT_Vec2f *p, void *data,
                        CT_MPool *pool) {
  CT_QTNode *c = ct_mpool_alloc(pool);
  CT_CHECK_MEM(c);
  clear_children(c);
  c->x = q->coords[(idx & 1) << 2];
  c->y = q->coords[((idx & 2) << 1) + 1];
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

CT_EXPORT int ct_qtree_init(CT_Quadtree *q, float x, float y, float w, float h,
                            size_t poolSize) {
  if (!ct_mpool_init(&q->pool, poolSize, sizeof(CT_QTNode))) {
    CT_QTNode *root = &q->root;
    clear_children(root);
    root->x = x;
    root->y = y;
    root->w = w;
    root->h = h;
    root->cx = x + w * 0.5f;
    root->cy = y + h * 0.5f;
    root->type = CT_TREE_EMPTY;
    return 0;
  }
  return 1;
}

CT_EXPORT void ct_qtree_free(CT_Quadtree *q) {
  ct_mpool_free_all(&q->pool);
}

static int insert_node(CT_QTNode *q, CT_Vec2f *p, void *data, CT_MPool *pool) {
  CT_QTNode *c;
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
          return insert_node(q, op, od, pool);
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

CT_EXPORT int ct_qtree_insert(CT_Quadtree *q, CT_Vec2f *p, void *data) {
  CT_CHECK(q != NULL, "tree is NULL");
  CT_CHECK(p != NULL, "point is NULL");
  if (insert_node(&q->root, p, data, &q->pool)) {
    q->size++;
    return 0;
  }
fail:
  return 1;
}

CT_EXPORT int ct_qtree_remove(CT_Quadtree *q, CT_Vec2f *p) {
  CT_CHECK(q != NULL, "tree is NULL");
  CT_CHECK(p != NULL, "point is NULL");
  CT_QTNode *path[24];
  CT_QTNode *node = &q->root;
  int d = path_for_point(node, p, path);
  switch (d) {
    case -1:
      return 1;
    case 0:
      node->point = node->data = NULL;
      node->type = CT_TREE_EMPTY;
      break;
    default:
      while (d > 0) {
        ct_mpool_free(&q->pool, path[d]);
        d--;
        node = path[d];
        node->children[child_index(node, p)] = NULL;
        if (node->children[0] != NULL || node->children[1] != NULL ||
            node->children[2] != NULL || node->children[3] != NULL) {
          break;
        }
        node->type = CT_TREE_EMPTY;
      }
  }
  return 0;
fail:
  return 1;
}

CT_EXPORT CT_QTNode *ct_qtree_find_leaf(CT_Quadtree *q, CT_Vec2f *p) {
  CT_CHECK(q != NULL, "tree is NULL");
  CT_CHECK(p != NULL, "point is NULL");
  CT_QTNode *c, *node = &q->root;
  size_t idx;
  while (node->type == CT_TREE_BRANCH) {
    idx = child_index(node, p);
    c = node->children[idx];
    if (c == NULL) {
      return NULL;
    }
    node = c;
  }
  return (node->type == CT_TREE_LEAF && ct_deltaeq2fv(node->point, p, EPS)
              ? node
              : NULL);
fail:
  return NULL;
}

CT_EXPORT void ct_qtree_trace_node(CT_QTNode *q, size_t depth) {
  if (q->point) {
    CT_INFO("d: %zu, %p b: [%f,%f,%f,%f] c: [%p,%p,%p,%p] t: %zu, p: (%f,%f)",
            depth, q, q->x, q->y, q->w, q->h, q->children[0], q->children[1],
            q->children[2], q->children[3], q->type, q->point->x, q->point->y);
  } else {
    CT_INFO("d: %zu, %p b: [%f,%f,%f,%f] c: [%p,%p,%p,%p] t: %zu", depth, q,
            q->x, q->y, q->w, q->h, q->children[0], q->children[1],
            q->children[2], q->children[3], q->type);
  }
}

static void trace_node_recursive(CT_QTNode *q, size_t depth) {
  if (q != NULL) {
    ct_qtree_trace_node(q, depth);
    for (size_t i = 0; i < 4; i++) {
      trace_node_recursive(q->children[i], depth + 1);
    }
  }
}

CT_EXPORT void ct_qtree_trace(CT_Quadtree *q) {
  trace_node_recursive(&q->root, 0);
}

static int visit_leaves(CT_QTNode *q, CT_QTVisitor visit, void *state) {
  switch (q->type) {
    case CT_TREE_LEAF:
      return visit(q, state);
    case CT_TREE_BRANCH:
      for (size_t i = 0; i < 4; i++) {
        if (q->children[i]) {
          int res = visit_leaves(q->children[i], visit, state);
          if (res) {
            return res;
          }
        }
      }
  }
  return 0;
}

CT_EXPORT int ct_qtree_visit_leaves(CT_Quadtree *q, CT_QTVisitor visit,
                                    void *state) {
  return visit_leaves(&q->root, visit, state);
}

static int visit_all(CT_QTNode *q, CT_QTVisitor visit, void *state) {
  int res = visit(q, state);
  if (res) {
    return res;
  }
  if (q->type == CT_TREE_BRANCH) {
    for (size_t i = 0; i < 4; i++) {
      if (q->children[i]) {
        visit_all(q->children[i], visit, state);
      }
    }
  }
  return 0;
}

CT_EXPORT int ct_qtree_visit(CT_Quadtree *q, CT_QTVisitor visit, void *state) {
  return visit_all(&q->root, visit, state);
}
