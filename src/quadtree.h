#pragma once

#include <stddef.h>

#include "mpool.h"
#include "vec.h"

enum { CT_QT_EMPTY = 0, CT_QT_BRANCH, CT_QT_LEAF };

typedef struct CT_QuadTree CT_QuadTree;

struct CT_QuadTree {
  CT_QuadTree *children[4];
  CT_Vec2f *point;
  void *data;
  union {
    struct {
      float x, cx, y, cy;
    };
    float coords[4];
  };
  size_t type;
};

typedef void (*CT_QuadTreeVisitor)(CT_QuadTree *, void *);

void ct_qtree_init(CT_QuadTree *q, float x, float y, float w, float h);
size_t ct_qtree_insert(CT_QuadTree *q, CT_Vec2f *p, void *data, CT_MPool *pool);
size_t ct_qtree_remove(CT_QuadTree *q, CT_Vec2f *p, CT_MPool *pool);
CT_QuadTree *ct_qtree_find_leaf(CT_QuadTree *q, CT_Vec2f *p);

void ct_qtree_trace_node(CT_QuadTree *q, size_t depth);
void ct_qtree_trace(CT_QuadTree *q, size_t depth);
void ct_qtree_visit_leaves(CT_QuadTree *q, CT_QuadTreeVisitor visit,
                           void *state);
