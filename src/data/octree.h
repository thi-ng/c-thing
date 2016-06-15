#pragma once

#include <stddef.h>

#include "config.h"
#include "data/tree.h"
#include "math/vec.h"
#include "mem/mpool.h"

CT_BEGIN_DECLS

typedef struct CT_OTNode CT_OTNode;

struct CT_OTNode {
  CT_OTNode *children[8];
  CT_Vec3f *point;
  void *data;
  union {
    struct {
      float x, cx, y, cy, z, cz, w, h, d;
    };
    float coords[9];
  };
  size_t type;
};

typedef struct {
  CT_OTNode root;
  CT_MPool pool;
  size_t size;
} CT_Octree;

typedef int (*CT_OTVisitor)(const CT_OTNode *, void *);

int ct_octree_init(CT_Octree *t, float x, float y, float z, float w, float h,
                   float d, size_t poolSize);
void ct_octree_free(CT_Octree *t);
int ct_octree_insert(CT_Octree *t, const CT_Vec3f *p, const void *data);
int ct_octree_remove(CT_Octree *t, const CT_Vec3f *p);
CT_OTNode *ct_octree_find_leaf(const CT_Octree *t, const CT_Vec3f *p);
void ct_octree_trace_node(const CT_OTNode *t, size_t depth);
void ct_octree_trace(const CT_Octree *t);
int ct_octree_visit_leaves(const CT_Octree *t, CT_OTVisitor visit, void *state);
void ct_octree_visit(const CT_Octree *t, CT_OTVisitor visit, void *state);

CT_END_DECLS
