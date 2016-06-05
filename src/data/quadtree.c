#include <stdlib.h>

#include "common/dbg.h"
#include "config.h"
#include "data/quadtree.h"

ct_inline size_t child_index(const CT_QTNode *node, const CT_Vec2f *p) {
  CT_CHECK(p->x >= node->x && p->x < node->x + node->w, "x bounds");
  CT_CHECK(p->y >= node->y && p->y < node->y + node->h, "y bounds");
  return (p->y < node->cy ? 0 : 2) + (p->x < node->cx ? 0 : 1);
fail:
  ct_qtree_trace_node(node, 0);
  exit(1);
}

ct_inline void clear_children(CT_QTNode *node) {
  node->children[0] = node->children[1] = node->children[2] =
      node->children[3]                 = NULL;
}

static int path_for_point(CT_QTNode *node, const CT_Vec2f *p,
                          CT_QTNode **path) {
  size_t i = 0;
  *path++  = node;
  while (node->type == CT_TREE_BRANCH) {
    node = node->children[child_index(node, p)];
    if (node == NULL) {
      return -1;
    }
    *path++ = node;
    i++;
  }
  return (node->type == CT_TREE_LEAF && ct_deltaeq2fv(node->point, p, EPS))
             ? i
             : -1;
}

static size_t make_leaf(CT_QTNode *node, size_t idx, CT_Vec2f *p, void *data,
                        CT_MPool *pool) {
  CT_QTNode *c = ct_mpool_alloc(pool);
  CT_CHECK_MEM(c);
  clear_children(c);
  c->x                = node->coords[(idx & 1) << 2];
  c->y                = node->coords[((idx & 2) << 1) + 1];
  c->w                = node->w * 0.5f;
  c->h                = node->h * 0.5f;
  c->cx               = c->x + c->w * 0.5;
  c->cy               = c->y + c->h * 0.5;
  c->type             = CT_TREE_LEAF;
  c->point            = p;
  c->data             = data;
  node->children[idx] = c;
  node->type          = CT_TREE_BRANCH;
  return 0;
fail:
  return 1;
}

CT_EXPORT int ct_qtree_init(CT_Quadtree *t, float x, float y, float w, float h,
                            size_t poolSize) {
  if (!ct_mpool_init(&t->pool, poolSize, sizeof(CT_QTNode))) {
    CT_QTNode *root = &t->root;
    clear_children(root);
    root->x    = x;
    root->y    = y;
    root->w    = w;
    root->h    = h;
    root->cx   = x + w * 0.5f;
    root->cy   = y + h * 0.5f;
    root->type = CT_TREE_EMPTY;
    return 0;
  }
  return 1;
}

CT_EXPORT void ct_qtree_free(CT_Quadtree *t) {
  ct_mpool_free_all(&t->pool);
}

static int insert_node(CT_QTNode *node, CT_Vec2f *p, void *data,
                       CT_MPool *pool) {
  CT_QTNode *c;
  size_t idx;
  while (node->type == CT_TREE_BRANCH) {
    idx = child_index(node, p);
    c   = node->children[idx];
    //ct_qtree_trace_node(node, 0);
    //CT_DEBUG("node: %p, child: %p, idx: %zu, t: %zu", node, c, idx, node->type);
    if (c == NULL) break;
    node = c;
  }
  switch (node->type) {
    case CT_TREE_EMPTY:
      node->point = p;
      node->data  = data;
      node->type  = CT_TREE_LEAF;
      return 0;
    case CT_TREE_LEAF:
      if (ct_deltaeq2fv(node->point, p, EPS)) {
        node->point = p;
        node->data  = data;
        return 0;
      } else {
        //CT_DEBUG("split leaf: %p", node);
        CT_Vec2f *op = node->point;
        void *od     = node->data;
        node->point = node->data = NULL;
        if (!make_leaf(node, child_index(node, p), p, data, pool)) {
          return insert_node(node, op, od, pool);
        }
        return 1;
      }
    case CT_TREE_BRANCH:
      return make_leaf(node, idx, p, data, pool);
    default:
      CT_SENTINEL("invalid node type: %zu", node->type);
  }
fail:
  return 1;
}

CT_EXPORT int ct_qtree_insert(CT_Quadtree *t, CT_Vec2f *p, void *data) {
  CT_CHECK(t != NULL, "tree is NULL");
  CT_CHECK(p != NULL, "point is NULL");
  if (!insert_node(&t->root, p, data, &t->pool)) {
    t->size++;
    return 0;
  }
fail:
  return 1;
}

CT_EXPORT int ct_qtree_remove(CT_Quadtree *t, CT_Vec2f *p) {
  CT_CHECK(t != NULL, "tree is NULL");
  CT_CHECK(p != NULL, "point is NULL");
  CT_QTNode *path[24];
  CT_QTNode *node = &t->root;
  int d           = path_for_point(node, p, path);
  switch (d) {
    case -1:
      return 1;
    case 0:
      node->point = node->data = NULL;
      node->type               = CT_TREE_EMPTY;
      break;
    default:
      while (d > 0) {
        ct_mpool_free(&t->pool, path[d]);
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
  t->size--;
  return 0;
fail:
  return 1;
}

CT_EXPORT CT_QTNode *ct_qtree_find_leaf(CT_Quadtree *t, CT_Vec2f *p) {
  CT_CHECK(t != NULL, "tree is NULL");
  CT_CHECK(p != NULL, "point is NULL");
  CT_QTNode *c, *node = &t->root;
  size_t idx;
  while (node->type == CT_TREE_BRANCH) {
    idx = child_index(node, p);
    c   = node->children[idx];
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

CT_EXPORT void ct_qtree_trace_node(const CT_QTNode *node, size_t depth) {
  if (node->point) {
    CT_INFO(
        "d: %zu, %p b: [%f,%f,%f,%f] [%f,%f] c: [%p,%p,%p,%p] t: %zu, p: "
        "(%f,%f)",
        depth, node, node->x, node->y, node->x + node->w, node->y + node->h,
        node->cx, node->cy, node->children[0], node->children[1],
        node->children[2], node->children[3], node->type, node->point->x,
        node->point->y);
  } else {
    CT_INFO("d: %zu, %p b: [%f,%f,%f,%f] [%f,%f] c: [%p,%p,%p,%p] t: %zu",
            depth, node, node->x, node->y, node->x + node->w, node->y + node->h,
            node->cx, node->cy, node->children[0], node->children[1],
            node->children[2], node->children[3], node->type);
  }
}

static void trace_node_recursive(CT_QTNode *node, size_t depth) {
  if (node != NULL) {
    ct_qtree_trace_node(node, depth);
    for (size_t i = 0; i < 4; i++) {
      trace_node_recursive(node->children[i], depth + 1);
    }
  }
}

CT_EXPORT void ct_qtree_trace(CT_Quadtree *t) {
  trace_node_recursive(&t->root, 0);
}

static int visit_leaves(CT_QTNode *node, CT_QTVisitor visit, void *state) {
  switch (node->type) {
    case CT_TREE_LEAF:
      return visit(node, state);
    case CT_TREE_BRANCH:
      for (size_t i = 0; i < 4; i++) {
        if (node->children[i]) {
          int res = visit_leaves(node->children[i], visit, state);
          if (res) {
            return res;
          }
        }
      }
  }
  return 0;
}

CT_EXPORT int ct_qtree_visit_leaves(CT_Quadtree *t, CT_QTVisitor visit,
                                    void *state) {
  return visit_leaves(&t->root, visit, state);
}

static void visit_all(CT_QTNode *node, CT_QTVisitor visit, void *state) {
  if (!visit(node, state) && node->type == CT_TREE_BRANCH) {
    for (size_t i = 0; i < 4; i++) {
      if (node->children[i]) {
        visit_all(node->children[i], visit, state);
      }
    }
  }
}

CT_EXPORT void ct_qtree_visit(CT_Quadtree *t, CT_QTVisitor visit, void *state) {
  visit_all(&t->root, visit, state);
}
