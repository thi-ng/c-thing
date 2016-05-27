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
  float x, y, cx, cy;
  size_t type;
};

typedef void (*CT_QuadTreeVisitor)(CT_QuadTree *, void *);

size_t ct_qtree_insert(CT_QuadTree *q, CT_Vec2f *p, void *data, CT_MPool *pool);
void ct_qtree_trace_node(CT_QuadTree *q, size_t depth);
void ct_qtree_trace(CT_QuadTree *q, size_t depth);
void ct_qtree_visit_leaves(CT_QuadTree *q, CT_QuadTreeVisitor visit, void *state);
