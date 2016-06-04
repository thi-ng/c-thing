#include "data/octree.h"
#include "common/dbg.h"
#include "config.h"

ct_inline size_t child_index(const CT_Octree *q, const CT_Vec3f *p) {
  return (p->z < q->cz ? 0 : 4) + (p->y < q->cy ? 0 : 2) +
         (p->x < q->cx ? 0 : 1);
}

ct_inline void clear_children(CT_Octree *q) {
  q->children[0] = q->children[1] = q->children[2] = q->children[3] =
      q->children[4] = q->children[5] = q->children[6] = q->children[7] = NULL;
}

static int path_for_point(CT_Octree *q, CT_Vec3f *p, CT_Octree **path) {
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
  return (q->type == CT_TREE_LEAF && ct_deltaeq3fv(q->point, p, EPS)) ? i : -1;
}

static size_t make_leaf(CT_Octree *q, size_t idx, CT_Vec3f *p, void *data,
                        CT_MPool *pool) {
  CT_Octree *c = CT_MP_ALLOC(pool, CT_Octree);
  CT_CHECK_MEM(c);
  clear_children(c);
  c->x = q->coords[idx & 1];
  c->y = q->coords[(idx >> 1) + 2];
  c->z = q->coords[(idx >> 2) + 4];
  c->w = q->w * 0.5f;
  c->h = q->h * 0.5f;
  c->d = q->d * 0.5f;
  c->cx = c->x + c->w;
  c->cy = c->y + c->h;
  c->cz = c->z + c->d;
  c->type = CT_TREE_LEAF;
  c->point = p;
  c->data = data;
  q->children[idx] = c;
  q->type = CT_TREE_BRANCH;
  return 0;
fail:
  return 1;
}

CT_EXPORT void ct_octree_init(CT_Octree *q, float x, float y, float z, float w,
                              float h, float d) {
  clear_children(q);
  q->x = x;
  q->y = y;
  q->z = z;
  q->w = w;
  q->h = h;
  q->d = d;
  q->cx = x + w * 0.5f;
  q->cy = y + h * 0.5f;
  q->cz = z + d * 0.5f;
  q->type = CT_TREE_EMPTY;
}

CT_EXPORT size_t ct_octree_insert(CT_Octree *q, CT_Vec3f *p, void *data,
                                  CT_MPool *pool) {
  CT_CHECK(q != NULL, "tree is NULL");
  CT_CHECK(p != NULL, "point is NULL");
  CT_Octree *c;
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
      if (ct_deltaeq3fv(q->point, p, EPS)) {
        q->point = p;
        q->data = data;
        return 0;
      } else {
        CT_Vec3f *op = q->point;
        void *od = q->data;
        q->point = q->data = NULL;
        if (!make_leaf(q, child_index(q, p), p, data, pool)) {
          return ct_octree_insert(q, op, od, pool);
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

size_t ct_octree_remove(CT_Octree *q, CT_Vec3f *p, CT_MPool *pool) {
  CT_Octree *path[24];
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
            q->children[2] != NULL || q->children[3] != NULL ||
            q->children[4] != NULL || q->children[5] != NULL ||
            q->children[6] != NULL || q->children[7] != NULL) {
          break;
        }
        q->type = CT_TREE_EMPTY;
      }
  }
  return 0;
}

CT_EXPORT CT_Octree *ct_octree_find_leaf(CT_Octree *q, CT_Vec3f *p) {
  CT_Octree *c;
  size_t idx;
  while (q->type == CT_TREE_BRANCH) {
    idx = child_index(q, p);
    c = q->children[idx];
    if (c == NULL) {
      return NULL;
    }
    q = c;
  }
  return ((q->type == CT_TREE_LEAF && ct_deltaeq3fv(q->point, p, EPS)) ? q
                                                                       : NULL);
}

CT_EXPORT void ct_octree_trace_node(CT_Octree *q, size_t depth) {
  if (q->point) {
    CT_INFO(
        "d: %zd: %p b: [%f,%f,%f,%f,%f,%f] c: [%p,%p,%p,%p,%p,%p,%p,%p] t: "
        "%zd, p: (%f,%f, %f)",
        depth, q, q->x, q->y, q->z, q->w, q->h, q->d, q->children[0],
        q->children[1], q->children[2], q->children[3], q->children[4],
        q->children[5], q->children[6], q->children[7], q->type, q->point->x,
        q->point->y, q->point->z);
  } else {
    CT_INFO(
        "d: %zd: %p b: [%f,%f,%f,%f,%f,%f] c: [%p,%p,%p,%p,%p,%p,%p,%p] t: %zd",
        depth, q, q->x, q->y, q->z, q->w, q->h, q->d, q->children[0],
        q->children[1], q->children[2], q->children[3], q->children[4],
        q->children[5], q->children[6], q->children[7], q->type);
  }
}

CT_EXPORT void ct_octree_trace(CT_Octree *q, size_t depth) {
  if (q != NULL) {
    ct_octree_trace_node(q, depth);
    for (size_t i = 0; i < 8; i++) {
      ct_octree_trace(q->children[i], depth + 1);
    }
  }
}

CT_EXPORT int ct_octree_visit_leaves(CT_Octree *q, CT_OctreeVisitor visit,
                                     void *state) {
  switch (q->type) {
    case CT_TREE_LEAF:
      return visit(q, state);
    case CT_TREE_BRANCH:
      for (size_t i = 0; i < 8; i++) {
        if (q->children[i]) {
          ct_octree_visit_leaves(q->children[i], visit, state);
        }
      }
  }
  return 0;
}

CT_EXPORT int ct_octree_visit(CT_Octree *q, CT_OctreeVisitor visit,
                              void *state) {
  int res = visit(q, state);
  if (res) {
    return res;
  }
  if (q->type == CT_TREE_BRANCH) {
    for (size_t i = 0; i < 8; i++) {
      if (q->children[i]) {
        ct_octree_visit(q->children[i], visit, state);
      }
    }
  }
  return 0;
}
