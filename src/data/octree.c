#include "data/octree.h"

ct_inline size_t child_index(const CT_OTNode *node, const CT_Vec3f *p) {
  CT_CHECK(p->x >= node->x && p->x < node->x + node->w, "x bounds");
  CT_CHECK(p->y >= node->y && p->y < node->y + node->h, "y bounds");
  CT_CHECK(p->z >= node->z && p->z < node->z + node->d, "z bounds");
  return ((p->z >= node->cz) << 2) | ((p->y >= node->cy) << 1) |
         (p->x >= node->cx);
fail:
  CT_INFO("p: [%f,%f,%f]", p->x, p->y, p->z);
  ct_octree_trace_node(node, 0);
  exit(1);
}

ct_inline void clear_children(CT_OTNode *node) {
  node->children[0] = node->children[1] = node->children[2] =
      node->children[3] = node->children[4] = node->children[5] =
          node->children[6] = node->children[7] = NULL;
}

static int path_for_point(CT_OTNode *node,
                          const CT_Vec3f *p,
                          CT_OTNode **path) {
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
  return (node->type == CT_TREE_LEAF && ct_deltaeq3fv(node->point, p, EPS))
             ? i
             : -1;
}

static size_t make_leaf(CT_OTNode *node,
                        size_t idx,
                        const CT_Vec3f *p,
                        const void *data,
                        CT_MPool *pool) {
  CT_OTNode *c;
  CT_CHECK_MEM(c = ct_mpool_alloc(pool));
  clear_children(c);
  c->x                = node->coords[idx & 1];
  c->y                = node->coords[((idx & 2) >> 1) | 2];
  c->z                = node->coords[(idx >> 2) | 4];
  c->w                = node->w * 0.5f;
  c->h                = node->h * 0.5f;
  c->d                = node->d * 0.5f;
  c->cx               = c->x + c->w * 0.5f;
  c->cy               = c->y + c->h * 0.5f;
  c->cz               = c->z + c->d * 0.5f;
  c->type             = CT_TREE_LEAF;
  c->point            = (CT_Vec3f *)p;
  c->data             = (void *)data;
  node->children[idx] = c;
  node->type          = CT_TREE_BRANCH;
  return 0;
fail:
  return 1;
}

ct_export int ct_octree_init(CT_Octree *t,
                             float x,
                             float y,
                             float z,
                             float w,
                             float h,
                             float d,
                             size_t poolSize) {
  if (!ct_mpool_init(&t->pool, poolSize, sizeof(CT_OTNode))) {
    CT_OTNode *root = &t->root;
    clear_children(root);
    root->x    = x;
    root->y    = y;
    root->z    = z;
    root->w    = w;
    root->h    = h;
    root->d    = d;
    root->cx   = x + w * 0.5f;
    root->cy   = y + h * 0.5f;
    root->cz   = z + d * 0.5f;
    root->type = CT_TREE_EMPTY;
    return 0;
  }
  return 1;
}

ct_export void ct_octree_free(CT_Octree *t) {
  ct_mpool_free(&t->pool);
}

static int insert_node(CT_OTNode *node,
                       const CT_Vec3f *p,
                       const void *data,
                       CT_MPool *pool) {
  CT_OTNode *c;
  size_t idx;
  while (node->type == CT_TREE_BRANCH) {
    idx = child_index(node, p);
    c   = node->children[idx];
    if (c == NULL) {
      break;
    }
    node = c;
  }
  switch (node->type) {
    case CT_TREE_EMPTY:
      node->point = (CT_Vec3f *)p;
      node->data  = (void *)data;
      node->type  = CT_TREE_LEAF;
      return 0;
    case CT_TREE_LEAF:
      if (ct_deltaeq3fv(node->point, p, EPS)) {
        node->point = (CT_Vec3f *)p;
        node->data  = (void *)data;
        return 0;
      } else {
        CT_Vec3f *op = node->point;
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

ct_export int ct_octree_insert(CT_Octree *t,
                               const CT_Vec3f *p,
                               const void *data) {
  CT_CHECK(t != NULL, "tree is NULL");
  CT_CHECK(p != NULL, "point is NULL");
  CT_OTNode *root = &t->root;
  CT_CHECK(p->x >= root->x && p->x < root->x + root->w, "x out of bounds");
  CT_CHECK(p->y >= root->y && p->y < root->y + root->h, "y out of bounds");
  CT_CHECK(p->z >= root->z && p->z < root->z + root->d, "z out of bounds");
  if (!insert_node(root, p, data, &t->pool)) {
    t->size++;
    return 0;
  }
fail:
  return 1;
}

ct_export int ct_octree_remove(CT_Octree *t, const CT_Vec3f *p) {
  CT_CHECK(t != NULL, "tree is NULL");
  CT_CHECK(p != NULL, "point is NULL");
  CT_OTNode *path[24];
  CT_OTNode *node = &t->root;
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
        ct_mpool_free_block(&t->pool, path[d]);
        d--;
        node = path[d];
        node->children[child_index(node, p)] = NULL;
        if (node->children[0] != NULL || node->children[1] != NULL ||
            node->children[2] != NULL || node->children[3] != NULL ||
            node->children[4] != NULL || node->children[5] != NULL ||
            node->children[6] != NULL || node->children[7] != NULL) {
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

ct_export CT_OTNode *ct_octree_find_leaf(const CT_Octree *t,
                                         const CT_Vec3f *p) {
  CT_CHECK(t != NULL, "tree is NULL");
  CT_CHECK(p != NULL, "point is NULL");
  CT_OTNode *c, *node = (CT_OTNode *)&t->root;
  size_t idx;
  while (node->type == CT_TREE_BRANCH) {
    idx = child_index(node, p);
    c   = node->children[idx];
    if (c == NULL) {
      return NULL;
    }
    node = c;
  }
  return ((node->type == CT_TREE_LEAF && ct_deltaeq3fv(node->point, p, EPS))
              ? node
              : NULL);
fail:
  return NULL;
}

ct_export void ct_octree_trace_node(const CT_OTNode *node, size_t depth) {
  if (node->point) {
    CT_INFO(
        "d: %zu: %p b: [%f,%f,%f,%f,%f,%f] [%f,%f,%f] c: "
        "[%p,%p,%p,%p,%p,%p,%p,%p] node: "
        "%zu, p: (%f,%f, %f)",
        depth, node, node->x, node->y, node->z, node->x + node->w,
        node->y + node->h, node->z + node->d, node->cx, node->cy, node->cz,
        node->children[0], node->children[1], node->children[2],
        node->children[3], node->children[4], node->children[5],
        node->children[6], node->children[7], node->type, node->point->x,
        node->point->y, node->point->z);
  } else {
    CT_INFO(
        "d: %zu: %p b: [%f,%f,%f,%f,%f,%f] [%f,%f,%f] c: "
        "[%p,%p,%p,%p,%p,%p,%p,%p] node: "
        "%zu",
        depth, node, node->x, node->y, node->z, node->x + node->w,
        node->y + node->h, node->z + node->d, node->cx, node->cy, node->cz,
        node->children[0], node->children[1], node->children[2],
        node->children[3], node->children[4], node->children[5],
        node->children[6], node->children[7], node->type);
  }
}

static void trace_node_recursive(const CT_OTNode *node, size_t depth) {
  if (node != NULL) {
    ct_octree_trace_node(node, depth);
    for (size_t i = 0; i < 8; i++) {
      trace_node_recursive(node->children[i], depth + 1);
    }
  }
}

ct_export void ct_octree_trace(const CT_Octree *t) {
  trace_node_recursive(&t->root, 0);
}

static int visit_leaves(const CT_OTNode *node,
                        CT_OTVisitor visit,
                        void *state) {
  switch (node->type) {
    case CT_TREE_LEAF:
      return visit(node, state);
    case CT_TREE_BRANCH:
      for (size_t i = 0; i < 8; i++) {
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

ct_export int ct_octree_visit_leaves(const CT_Octree *t,
                                     CT_OTVisitor visit,
                                     void *state) {
  return visit_leaves(&t->root, visit, state);
}

static void visit_all(const CT_OTNode *node, CT_OTVisitor visit, void *state) {
  if (!visit(node, state) && node->type == CT_TREE_BRANCH) {
    for (size_t i = 0; i < 8; i++) {
      if (node->children[i]) {
        visit_all(node->children[i], visit, state);
      }
    }
  }
}

ct_export void ct_octree_visit(const CT_Octree *t,
                               CT_OTVisitor visit,
                               void *state) {
  visit_all(&t->root, visit, state);
}
