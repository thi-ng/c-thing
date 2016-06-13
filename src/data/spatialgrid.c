#include <stddef.h>
#include <stdlib.h>

#include "common/dbg.h"
#include "data/spatialgrid.h"
#include "math/vec.h"

struct CT_SPCell {
  void *value;
  CT_SPCell *next;
  float key[3];
};

ct_inline size_t find_cell_idx(const CT_SpatialGrid *grid, const float *p,
                               size_t idx) {
  return (size_t)((p[idx] - grid->offsets[idx]) * grid->invStrides[idx]);
}

static size_t find_cell1d(const CT_SpatialGrid *grid, const float *p) {
  return find_cell_idx(grid, p, 0);
}

static size_t find_cell2d(const CT_SpatialGrid *grid, const float *p) {
  return find_cell_idx(grid, p, 1) * grid->strides[0] +
         find_cell_idx(grid, p, 0);
}

static size_t find_cell3d(const CT_SpatialGrid *grid, const float *p) {
  return find_cell_idx(grid, p, 2) * grid->strides[1] +
         find_cell_idx(grid, p, 1) * grid->strides[0] +
         find_cell_idx(grid, p, 0);
}

CT_EXPORT int ct_spgrid_init(CT_SpatialGrid *grid, float *start, float *end,
                             size_t *strides, size_t dims, size_t poolSize) {
  CT_CHECK(dims > 0 && dims < 4, "wrong dimension: %zu", dims);
  size_t numCells = strides[0];
  for (size_t i = 0; i < dims; i++) {
    grid->widths[i]     = end[i] - start[i];
    grid->offsets[i]    = start[i];
    grid->invStrides[i] = (float)strides[i] / grid->widths[i];
    if (i > 0) {
      numCells *= strides[i];
    }
    grid->strides[i] = numCells;
  }
  grid->numCells = numCells;
  grid->dims     = dims;
  grid->find_cell =
      dims == 1 ? find_cell1d : dims == 2 ? find_cell2d : find_cell3d;
  if (!ct_mpool_init(&grid->pool, MAX(numCells, poolSize), sizeof(CT_SPCell))) {
    CT_CHECK_MEM(grid->cells = calloc(numCells, sizeof(CT_SPCell *)));
    return 0;
  }
fail:
  return 1;
}

CT_EXPORT void ct_spgrid_free(CT_SpatialGrid *grid) {
  free(grid->cells);
  ct_mpool_free_all(&grid->pool);
}

CT_EXPORT int ct_spgrid_insert(CT_SpatialGrid *grid, float *p, void *item) {
  size_t id = grid->find_cell(grid, p);
  CT_CHECK(id >= 0 && id < grid->numCells, "p out of bounds");
  CT_SPCell *cell = ct_mpool_alloc(&grid->pool);
  CT_CHECK_MEM(cell);
  CT_DEBUG("insert @ grid: %zu new: (%p) -> %p", id, cell, grid->cells[id]);
  cell->value  = item;
  cell->next   = grid->cells[id];
  cell->key[0] = p[0];
  if (grid->dims > 1) {
    cell->key[1] = p[1];
    if (grid->dims > 2) {
      cell->key[2] = p[2];
    }
  }
  grid->cells[id] = cell;
  return 0;
fail:
  return 1;
}

CT_EXPORT int ct_spgrid_remove(CT_SpatialGrid *grid, float *p, void *item) {
  size_t id = grid->find_cell(grid, p);
  CT_CHECK(id >= 0 && id < grid->numCells, "p out of bounds");
  CT_SPCell *cell = grid->cells[id], *prev = NULL;
  while (cell) {
    if (cell->value == item) {
      CT_DEBUG("remove @ grid: %zu, %p -> (%p) -> %p", id, prev, cell,
               cell->next);
      if (prev == NULL) {
        grid->cells[id] = cell->next;
      } else {
        prev->next = cell->next;
      }
      ct_mpool_free(&grid->pool, cell);
      return 0;
    }
    prev = cell;
    cell = cell->next;
  }
fail:
  return 1;
}

CT_EXPORT int ct_spgrid_update(CT_SpatialGrid *grid, float *p1, float *p2,
                               void *item) {
  size_t id1 = grid->find_cell(grid, p1);
  size_t id2 = grid->find_cell(grid, p2);
  if (id1 != id2) {
    return !ct_spgrid_remove(grid, p1, item) &&
           ct_spgrid_insert(grid, p2, item);
  }
  return 0;
}

static size_t select1d(const CT_SpatialGrid *grid, size_t s, size_t e,
                       size_t idx, float x1, float x2, size_t i, void **results,
                       size_t len) {
  while (s <= e) {
    CT_SPCell *cell = grid->cells[s];
    while (cell) {
      if (cell->key[idx] >= x1 && cell->key[idx] <= x2) {
        CT_DEBUG("sel @ %zu: %p = %p / %p -> %p", s, &results[i], cell->value,
                 cell, cell->next);
        results[i] = cell->value;
        i++;
        if (i == len) {
          return i;
        }
      }
      cell = cell->next;
    }
    s++;
  }
  return i;
}

CT_EXPORT size_t ct_spgrid_select1d(CT_SpatialGrid *grid, float p, float eps,
                                    void **results, size_t len) {
  size_t i = 0;
  float x1 = p - eps;
  float x2 = p + eps;
  size_t s = find_cell_idx(grid, &x1, 0);
  size_t e = find_cell_idx(grid, &x2, 0);
  s        = MAX(s, 0);
  e        = MIN(e, grid->strides[0] - 1);
  return select1d(grid, s, e, 0, x1, x2, 0, results, len);
}

CT_EXPORT size_t ct_spgrid_select2d(CT_SpatialGrid *grid, float *p, float *eps,
                                    void **results, size_t len) {
  size_t i = 0;
  CT_Vec2f a, b;
  ct_sub2fv((CT_Vec2f *)p, (CT_Vec2f *)eps, &a);
  ct_add2fv((CT_Vec2f *)p, (CT_Vec2f *)eps, &b);
  size_t s = find_cell2d(grid, (float *)&a);
  size_t e = find_cell2d(grid, (float *)&b);
  s        = MAX(s, 0);
  e        = MIN(e, grid->strides[1] - 1);
  while (s <= e && i < len) {
    s++;
  }
  return i;
}
