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
  size_t sizes[3];
  float offsets[3];
  float invWidths[3];
  int (*find_cell)(const CT_SpatialGrid *, const float *);
};

int ct_spgrid_init(CT_SpatialGrid *grid, const float *start, const float *end,
                   const size_t *strides, size_t dims, size_t poolSize);
void ct_spgrid_free(CT_SpatialGrid *grid);
int ct_spgrid_insert(CT_SpatialGrid *grid, const float *p, const void *item);
int ct_spgrid_remove(CT_SpatialGrid *grid, const float *p, const void *item);
int ct_spgrid_update(CT_SpatialGrid *grid, const float *p1, const float *p2,
                     const void *item);
size_t ct_spgrid_select1d(const CT_SpatialGrid *grid, float p, float eps,
                          void **results, size_t len);
size_t ct_spgrid_select2d(const CT_SpatialGrid *grid, const float *p,
                          const float *eps, void **results, size_t len);
size_t ct_spgrid_select3d(const CT_SpatialGrid *grid, const float *p,
                          const float *eps, void **results, size_t len);

void ct_spgrid_trace(const CT_SpatialGrid *grid);

CT_END_DECLS
