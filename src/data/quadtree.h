#pragma once

#include <stddef.h>

#include "data/tree.h"
#include "geom/rect.h"
#include "math/vec.h"
#include "mem/mpool.h"

typedef struct CT_QTNode CT_QTNode;

struct CT_QTNode {
  CT_QTNode *children[4];
  CT_Vec2f *point;
  void *data;
  union {
    struct {
      float x, y, w, h, cx, cy;
    };
    struct {
      CT_Vec2f pos, size, centroid;
    };
    CT_Rect2f bounds;
    float coords[6];
  };
  size_t type;
};

typedef struct {
  CT_MPool pool;
  CT_QTNode root;
  size_t size;
} CT_Quadtree;

typedef int (*CT_QTVisitor)(CT_QTNode *, void *);

int ct_qtree_init(CT_Quadtree *q, float x, float y, float w, float h,
                  size_t poolSize);
void ct_qtree_free(CT_Quadtree *q);
int ct_qtree_insert(CT_Quadtree *q, CT_Vec2f *p, void *data);
int ct_qtree_remove(CT_Quadtree *q, CT_Vec2f *p);
CT_QTNode *ct_qtree_find_leaf(CT_Quadtree *q, CT_Vec2f *p);
void ct_qtree_trace_node(CT_QTNode *q, size_t depth);
void ct_qtree_trace(CT_Quadtree *q);
int ct_qtree_visit_leaves(CT_Quadtree *q, CT_QTVisitor visit, void *state);
int ct_qtree_visit(CT_Quadtree *q, CT_QTVisitor visit, void *state);
