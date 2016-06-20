#pragma once

#include <stddef.h>

#include "cthing.h"
#include "data/tree.h"
#include "geom/rect.h"
#include "math/vec.h"
#include "mem/mpool.h"

CT_BEGIN_DECLS

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

typedef int (*CT_QTVisitor)(const CT_QTNode *, void *);

int ct_qtree_init(CT_Quadtree *t, float x, float y, float w, float h,
                  size_t poolSize);
void ct_qtree_free(CT_Quadtree *t);
int ct_qtree_insert(CT_Quadtree *t, const CT_Vec2f *p, const void *data);
int ct_qtree_remove(CT_Quadtree *t, const CT_Vec2f *p);
CT_QTNode *ct_qtree_find_leaf(const CT_Quadtree *t, const CT_Vec2f *p);
void ct_qtree_trace_node(const CT_QTNode *node, size_t depth);
void ct_qtree_trace(const CT_Quadtree *t);
int ct_qtree_visit_leaves(const CT_Quadtree *t, CT_QTVisitor visit,
                          void *state);
void ct_qtree_visit(const CT_Quadtree *t, CT_QTVisitor visit, void *state);

CT_END_DECLS
