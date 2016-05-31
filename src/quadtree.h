#pragma once

#include <stddef.h>

#include "mpool.h"
#include "tree.h"
#include "vec.h"

typedef struct CT_Quadtree CT_Quadtree;

struct CT_Quadtree {
  CT_Quadtree *children[4];
  CT_Vec2f *point;
  void *data;
  union {
    struct {
      float x, cx, y, cy, w, h;
    };
    float coords[6];
  };
  size_t type;
};

typedef int (*CT_QuadtreeVisitor)(CT_Quadtree *, void *);

void ct_qtree_init(CT_Quadtree *q, float x, float y, float w, float h);
size_t ct_qtree_insert(CT_Quadtree *q, CT_Vec2f *p, void *data, CT_MPool *pool);
size_t ct_qtree_remove(CT_Quadtree *q, CT_Vec2f *p, CT_MPool *pool);
CT_Quadtree *ct_qtree_find_leaf(CT_Quadtree *q, CT_Vec2f *p);

void ct_qtree_trace_node(CT_Quadtree *q, size_t depth);
void ct_qtree_trace(CT_Quadtree *q, size_t depth);
int ct_qtree_visit_leaves(CT_Quadtree *q, CT_QuadtreeVisitor visit,
                          void *state);
int ct_qtree_visit(CT_Quadtree *q, CT_QuadtreeVisitor visit, void *state);
