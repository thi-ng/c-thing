#pragma once

#include <stddef.h>

#include "mpool.h"
#include "tree.h"
#include "vec.h"

typedef struct CT_Octree CT_Octree;

struct CT_Octree {
  CT_Octree *children[8];
  CT_Vec3f *point;
  void *data;
  union {
    struct {
      float x, cx, y, cy, z, cz;
    };
    float coords[6];
  };
  size_t type;
};

typedef void (*CT_OctreeVisitor)(CT_Octree *, void *);

void ct_octree_init(CT_Octree *q, float x, float y, float z, float w, float h,
                    float d);
size_t ct_octree_insert(CT_Octree *q, CT_Vec3f *p, void *data, CT_MPool *pool);
size_t ct_octree_remove(CT_Octree *q, CT_Vec3f *p, CT_MPool *pool);
CT_Octree *ct_octree_find_leaf(CT_Octree *q, CT_Vec3f *p);

void ct_octree_trace_node(CT_Octree *q, size_t depth);
void ct_octree_trace(CT_Octree *q, size_t depth);
void ct_octree_visit_leaves(CT_Octree *q, CT_OctreeVisitor visit, void *state);
