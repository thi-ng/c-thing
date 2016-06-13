#pragma once

#include "config.h"
#include "math/math.h"
#include "mem/mpool.h"

CT_BEGIN_DECLS

typedef struct CT_SPCell CT_SPCell;

typedef struct CT_SpatialGrid CT_SpatialGrid;

struct CT_SpatialGrid {
  CT_MPool pool;
  CT_SPCell **cells;
  size_t numCells;
  size_t dims;
  size_t strides[3];
  float invStrides[3];
  float offsets[3];
  float widths[3];
  size_t (*find_cell)(const CT_SpatialGrid *, const float *);
};

int ct_spgrid_init(CT_SpatialGrid *grid, float *start, float *end,
                   size_t *strides, size_t dims, size_t poolSize);
void ct_spgrid_free(CT_SpatialGrid *grid);
int ct_spgrid_insert(CT_SpatialGrid *grid, float *p, void *item);
int ct_spgrid_remove(CT_SpatialGrid *grid, float *p, void *item);
int ct_spgrid_update(CT_SpatialGrid *grid, float *p1, float *p2, void *item);
size_t ct_spgrid_select1d(CT_SpatialGrid *grid, float p, float eps,
                          void **results, size_t len);
size_t ct_spgrid_select2d(CT_SpatialGrid *grid, float *p, float *eps,
                          void **results, size_t len);

CT_END_DECLS
