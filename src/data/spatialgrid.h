#pragma once

#include "config.h"
#include "math/math.h"
#include "mem/mpool.h"

CT_BEGIN_DECLS

typedef struct CT_SPB CT_SPB;

typedef struct CT_SpatialGrid CT_SpatialGrid;

struct CT_SpatialGrid {
  CT_MPool pool;
  CT_SPB **cells;
  size_t numCells;
  size_t strides[4];
  float invStrides[4];
  float offsets[4];
  float widths[4];
  size_t (*find_cell)(const CT_SpatialGrid *, const float *);
};

int ct_spgrid_init(CT_SpatialGrid *bin, float *start, float *end,
                   size_t *strides, size_t dims, size_t poolSize);
void ct_spgrid_free(CT_SpatialGrid *bin);
int ct_spgrid_insert(CT_SpatialGrid *bin, float *p, void *item);
int ct_spgrid_remove(CT_SpatialGrid *bin, float *p, void *item);
int ct_spgrid_update(CT_SpatialGrid *bin, float *p1, float *p2, void *item);
size_t ct_spgrid_select(CT_SpatialGrid *bin, float *p, float eps,
                        void **results, size_t len);

CT_END_DECLS
