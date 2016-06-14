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

ct_inline int find_cell_idx(const CT_SpatialGrid *grid, const float *p,
                            size_t idx) {
  return (int)((p[idx] - grid->offsets[idx]) * grid->invWidths[idx]);
}

static int find_cell1d(const CT_SpatialGrid *grid, const float *p) {
  return find_cell_idx(grid, p, 0);
}

static int find_cell2d(const CT_SpatialGrid *grid, const float *p) {
  return find_cell_idx(grid, p, 1) * grid->strides[0] +
         find_cell_idx(grid, p, 0);
}

static int find_cell3d(const CT_SpatialGrid *grid, const float *p) {
  return find_cell_idx(grid, p, 2) * grid->strides[1] +
         find_cell_idx(grid, p, 1) * grid->strides[0] +
         find_cell_idx(grid, p, 0);
}

CT_EXPORT int ct_spgrid_init(CT_SpatialGrid *grid, const float *start,
                             const float *end, const size_t *strides,
                             size_t dims, size_t poolSize) {
  CT_CHECK(dims > 0 && dims < 4, "wrong dimension: %zu", dims);
  size_t numCells = strides[0];
  for (size_t i = 0; i < dims; i++) {
    grid->offsets[i]   = start[i];
    grid->sizes[i]     = strides[i];
    grid->invWidths[i] = (float)strides[i] / (end[i] - start[i]);
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

CT_EXPORT int ct_spgrid_insert(CT_SpatialGrid *grid, const float *p,
                               const void *item) {
  int id = grid->find_cell(grid, p);
  CT_CHECK(id >= 0 && id < grid->numCells, "p out of bounds");
  CT_SPCell *cell = ct_mpool_alloc(&grid->pool);
  CT_CHECK_MEM(cell);
  CT_DEBUG("insert @ grid: %d new: (%p) -> %p", id, cell, grid->cells[id]);
  cell->value  = (void *)item;
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

CT_EXPORT int ct_spgrid_remove(CT_SpatialGrid *grid, const float *p,
                               const void *item) {
  int id = grid->find_cell(grid, p);
  CT_CHECK(id >= 0 && id < grid->numCells, "p out of bounds");
  CT_SPCell *cell = grid->cells[id], *prev = NULL;
  while (cell) {
    if (cell->value == item) {
      CT_DEBUG("remove @ grid: %d, %p -> (%p) -> %p", id, prev, cell,
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

CT_EXPORT int ct_spgrid_update(CT_SpatialGrid *grid, const float *p,
                               const float *q, const void *item) {
  int id  = grid->find_cell(grid, p);
  int id2 = grid->find_cell(grid, q);
  if (id != id2) {
    return !ct_spgrid_remove(grid, p, item) && ct_spgrid_insert(grid, q, item);
  }
  CT_SPCell *cell = grid->cells[id];
  while (cell) {
    if (cell->value == item) {
      CT_DEBUG("update @ grid: %d, (%p) -> %p", id, cell, cell->next);
      cell->key[0] = q[0];
      if (grid->dims > 1) {
        cell->key[1] = q[1];
        if (grid->dims > 2) {
          cell->key[2] = q[2];
        }
      }
      return 0;
    }
    cell = cell->next;
  }
  return 1;
}

CT_EXPORT size_t ct_spgrid_select1d(const CT_SpatialGrid *grid, float p,
                                    float eps, void **results, size_t len) {
  size_t i = 0;
  float x1 = p - eps;
  float x2 = p + eps;
  int s    = find_cell_idx(grid, &x1, 0);
  int e    = find_cell_idx(grid, &x2, 0);
  s        = MAX(s, 0);
  MIN_LET(int, e, e, grid->sizes[0] - 1);
  CT_DEBUG("select range: %d,%d", s, e);
  while (s <= e) {
    CT_SPCell *cell = grid->cells[s];
    while (cell) {
      if (cell->key[0] >= x1 && cell->key[0] <= x2) {
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

CT_EXPORT size_t ct_spgrid_select2d(const CT_SpatialGrid *grid, const float *p,
                                    const float *eps, void **results,
                                    size_t len) {
  size_t i = 0;
  CT_Vec2f a, b;
  ct_sub2fv((CT_Vec2f *)p, (CT_Vec2f *)eps, &a);
  ct_add2fv((CT_Vec2f *)p, (CT_Vec2f *)eps, &b);
  int sx = find_cell_idx(grid, (float *)&a, 0);
  int ex = find_cell_idx(grid, (float *)&b, 0);
  int sy = find_cell_idx(grid, (float *)&a, 1);
  int ey = find_cell_idx(grid, (float *)&b, 1);
  sx     = MAX(sx, 0);
  sy     = MAX(sy, 0);
  MIN_LET(int, ex, ex, grid->sizes[0] - 1);
  MIN_LET(int, ey, ey, grid->sizes[1] - 1);
  CT_DEBUG("select range: [%d,%d] - [%d,%d]", sx, sy, ex, ey);
  while (sy <= ey) {
    size_t s = sy * grid->strides[0] + sx;
    size_t e = sy * grid->strides[0] + ex;
    while (s <= e) {
      CT_SPCell *cell = grid->cells[s];
      while (cell) {
        if (cell->key[1] >= a.y && cell->key[1] <= b.y && cell->key[0] >= a.x &&
            cell->key[0] <= b.x) {
          CT_DEBUG("sel @ %zu: %p = %p / %p -> %p", s + sx, &results[i],
                   cell->value, cell, cell->next);
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
    sy++;
  }
  return i;
}

CT_EXPORT void ct_spgrid_trace(const CT_SpatialGrid *grid) {
  switch (grid->dims) {
    case 1:
      CT_INFO("size: %zu, stride: %zu, inv: %f", grid->sizes[0],
              grid->strides[0], grid->invWidths[0]);
      break;
    case 2:
      CT_INFO("size: [%zu, %zu], stride: [%zu, %zu], inv: [%f, %f]",
              grid->sizes[0], grid->sizes[1], grid->strides[0],
              grid->strides[1], grid->invWidths[0], grid->invWidths[1]);
      break;
    case 3:
      CT_INFO(
          "size: [%zu, %zu, %zu], stride: [%zu, %zu, %zu], inv: [%f, %f, %f]",
          grid->sizes[0], grid->sizes[1], grid->sizes[2], grid->strides[0],
          grid->strides[1], grid->strides[2], grid->invWidths[0],
          grid->invWidths[1], grid->invWidths[2]);
  }
}
