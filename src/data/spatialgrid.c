#include "data/spatialgrid.h"
#include "math/vec.h"
#include "mem/mem.h"

struct CT_SPCell {
  void *value;
  CT_SPCell *next;
  float key[3];
};

ct_inline int find_cell_idx(const CT_SpatialGrid *grid,
                            const float *p,
                            size_t idx) {
  return (int)((p[idx] - grid->offset[idx]) * grid->invWidth[idx]);
}

static int find_cell1d(const CT_SpatialGrid *grid, const float *p) {
  return find_cell_idx(grid, p, 0);
}

static int find_cell2d(const CT_SpatialGrid *grid, const float *p) {
  return find_cell_idx(grid, p, 1) * grid->stride[0] +
         find_cell_idx(grid, p, 0);
}

static int find_cell3d(const CT_SpatialGrid *grid, const float *p) {
  return find_cell_idx(grid, p, 2) * grid->stride[1] +
         find_cell_idx(grid, p, 1) * grid->stride[0] +
         find_cell_idx(grid, p, 0);
}

ct_export int ct_spgrid_init(CT_SpatialGrid *grid,
                             const float *start,
                             const float *end,
                             const size_t *stride,
                             size_t dims,
                             size_t poolSize) {
  CT_CHECK(dims > 0 && dims < 4, "wrong dimension: %zu", dims);
  size_t numCells = stride[0];
  for (size_t i = 0; i < dims; i++) {
    grid->offset[i]   = start[i];
    grid->limit[i]    = end[i];
    grid->size[i]     = stride[i];
    grid->invWidth[i] = (float)stride[i] / (end[i] - start[i]);
    if (i > 0) {
      numCells *= stride[i];
    }
    grid->stride[i] = numCells;
  }
  grid->numCells = numCells;
  grid->dims     = dims;
  grid->find_cell =
      dims == 1 ? find_cell1d : dims == 2 ? find_cell2d : find_cell3d;
  if (!ct_mpool_init(&grid->pool, CT_MAX(numCells >> 1, poolSize),
                     sizeof(CT_SPCell))) {
    grid->cells = calloc(numCells, sizeof(CT_SPCell *));
    CT_CHECK_MEM(grid);
    return 0;
  }
fail:
  return 1;
}

ct_export void ct_spgrid_free(CT_SpatialGrid *grid) {
  free(grid->cells);
  ct_mpool_free(&grid->pool);
}

static int do_insert(CT_SpatialGrid *grid,
                     int id,
                     const float *p,
                     const void *item) {
  CT_CHECK(id >= 0 && id < grid->numCells, "p out of bounds");
  CT_SPCell *cell = ct_mpool_alloc(&grid->pool);
  CT_CHECK_MEM(cell);
  CT_DEBUG("insert @ grid: %d new: (%p) -> %p", id, cell, grid->cells[id]);
  cell->value     = (void *)item;
  cell->next      = grid->cells[id];
  grid->cells[id] = cell;
  cell->key[0]    = p[0];
  if (grid->dims > 1) {
    cell->key[1] = p[1];
    if (grid->dims > 2) {
      cell->key[2] = p[2];
    }
  }
  return 0;
fail:
  return 1;
}

ct_export int ct_spgrid_insert(CT_SpatialGrid *grid,
                               const float *p,
                               const void *item) {
  return do_insert(grid, grid->find_cell(grid, p), p, item);
}

static int do_remove(CT_SpatialGrid *grid, int id, const void *item) {
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
      ct_mpool_free_block(&grid->pool, cell);
      return 0;
    }
    prev = cell;
    cell = cell->next;
  }
fail:
  return 1;
}

ct_export int ct_spgrid_remove(CT_SpatialGrid *grid,
                               const float *p,
                               const void *item) {
  return do_remove(grid, grid->find_cell(grid, p), item);
}

ct_export int ct_spgrid_update(CT_SpatialGrid *grid,
                               const float *p,
                               const float *q,
                               const void *item) {
  int id  = grid->find_cell(grid, p);
  int id2 = grid->find_cell(grid, q);
  if (id != id2) {
    return !do_remove(grid, id, item) && do_insert(grid, id2, q, item);
  }
  CT_CHECK(id >= 0 && id < grid->numCells, "p out of bounds");
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
fail:
  return 1;
}

ct_export size_t ct_spgrid_select1d(const CT_SpatialGrid *grid,
                                    float p,
                                    float eps,
                                    void **results,
                                    size_t len) {
  size_t i = 0;
  float x1 = p - eps;
  float x2 = p + eps;
  int s    = find_cell_idx(grid, &x1, 0);
  int tex  = find_cell_idx(grid, &x2, 0);
  s        = CT_MAX(s, 0);
  tex      = ct_mini(tex, grid->size[0] - 1);
  CT_DEBUG("select range: %d,%d", s, tex);
  while (s <= tex) {
    CT_SPCell *cell = grid->cells[s];
    while (cell) {
      if (cell->key[0] >= x1 && cell->key[0] <= x2) {
        CT_DEBUG("sel @ %d: %p = %p / %p -> %p", s, &results[i], cell->value,
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

ct_export size_t ct_spgrid_select2d(const CT_SpatialGrid *grid,
                                    const float *p,
                                    const float *eps,
                                    void **results,
                                    size_t len) {
  size_t i = 0;
  CT_Vec2f a, b;
  ct_sub2fv((CT_Vec2f *)p, (CT_Vec2f *)eps, &a);
  ct_add2fv((CT_Vec2f *)p, (CT_Vec2f *)eps, &b);
  int sx = find_cell_idx(grid, a.buf, 0);
  int ex = find_cell_idx(grid, b.buf, 0);
  int sy = find_cell_idx(grid, a.buf, 1);
  int ey = find_cell_idx(grid, b.buf, 1);
  sx     = CT_MAX(sx, 0);
  sy     = CT_MAX(sy, 0);
  ex     = ct_mini(ex, grid->size[0] - 1);
  ey     = ct_mini(ey, grid->size[1] - 1);
  CT_DEBUG("select range: [%d,%d] - [%d,%d]", sx, sy, ex, ey);
  const size_t stride = grid->stride[0];
  while (sy <= ey) {
    size_t s         = sy * stride + sx;
    const size_t tex = sy * stride + ex;
    while (s <= tex) {
      CT_SPCell *cell = grid->cells[s];
      while (cell) {
        const float *key = cell->key;
        if (key[1] >= a.y && key[1] <= b.y && key[0] >= a.x && key[0] <= b.x) {
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

ct_export size_t ct_spgrid_select3d(const CT_SpatialGrid *grid,
                                    const float *p,
                                    const float *eps,
                                    void **results,
                                    size_t len) {
  size_t i = 0;
  CT_Vec3f a, b, veps;
#ifdef CT_FEATURE_SSE
  ct_set3fxyz(&veps, eps[0], eps[1], eps[2]);
  ct_set3fxyz(&b, p[0], p[1], p[2]);
  ct_sub3fv(&b, &veps, &a);
  ct_add3fv_imm(&b, &veps);
#else
  ct_sub3fv((CT_Vec3f *)p, (CT_Vec3f *)eps, &a);
  ct_add3fv((CT_Vec3f *)p, (CT_Vec3f *)eps, &b);
#endif
  int sx = find_cell_idx(grid, a.buf, 0);
  int sy = find_cell_idx(grid, a.buf, 1);
  int sz = find_cell_idx(grid, a.buf, 2);
  int ex = find_cell_idx(grid, b.buf, 0);
  int ey = find_cell_idx(grid, b.buf, 1);
  int ez = find_cell_idx(grid, b.buf, 2);
  sx     = CT_MAX(sx, 0);
  sy     = CT_MAX(sy, 0);
  sz     = CT_MAX(sz, 0);
  ex     = ct_mini(ex, grid->size[0] - 1);
  ey     = ct_mini(ey, grid->size[1] - 1);
  ez     = ct_mini(ez, grid->size[2] - 1);
  CT_DEBUG("select range: [%d,%d,%d] - [%d,%d,%d]", sx, sy, sz, ex, ey, ez);
  const size_t stridex = grid->stride[0];
  const size_t stridey = grid->stride[1];
  while (sz <= ez) {
    size_t tsy       = sz * stridey + sy * stridex;
    const size_t tey = sz * stridey + ey * stridex;
    CT_DEBUG("z: %d tsy: %zu - %zu", sz, tsy, tey);
    while (tsy <= tey) {
      size_t tsx       = tsy + sx;
      const size_t tex = tsy + ex;
      while (tsx <= tex) {
        CT_DEBUG("check cell: %zu", tsx);
        CT_SPCell *cell = grid->cells[tsx];
        while (cell) {
          const float *key = cell->key;
          if (key[2] >= a.z && key[2] <= b.z && key[1] >= a.y &&
              key[1] <= b.y && key[0] >= a.x && key[0] <= b.x) {
            CT_DEBUG("sel @ %zu: %p = %p / %p -> %p", tsx, &results[i],
                     cell->value, cell, cell->next);
            results[i] = cell->value;
            i++;
            if (i == len) {
              return i;
            }
          }
          cell = cell->next;
        }
        tsx++;
      }
      tsy += stridex;
    }
    sz++;
  }
  return i;
}

ct_export void ct_spgrid_trace(const CT_SpatialGrid *grid) {
  switch (grid->dims) {
    case 1:
      CT_INFO("size: %zu, stride: %zu, inv: %f bounds: [%f, %f]", grid->size[0],
              grid->stride[0], grid->invWidth[0], grid->offset[0],
              grid->limit[0]);
      break;
    case 2:
      CT_INFO(
          "size: [%zu, %zu], stride: [%zu, %zu], inv: [%f, %f], bounds: [%f,%f "
          "%f,%f]",
          grid->size[0], grid->size[1], grid->stride[0], grid->stride[1],
          grid->invWidth[0], grid->invWidth[1], grid->offset[0],
          grid->offset[1], grid->limit[0], grid->limit[1]);
      break;
    case 3:
      CT_INFO(
          "size: [%zu, %zu, %zu], stride: [%zu, %zu, %zu], inv: [%f, %f, %f], "
          "bounds: [%f,%f,%f %f,%f,%f]",
          grid->size[0], grid->size[1], grid->size[2], grid->stride[0],
          grid->stride[1], grid->stride[2], grid->invWidth[0],
          grid->invWidth[1], grid->invWidth[2], grid->offset[0],
          grid->offset[1], grid->offset[2], grid->limit[0], grid->limit[1],
          grid->limit[2]);
  }
}
