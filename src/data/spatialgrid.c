#include <stddef.h>
#include <stdlib.h>

#include "common/dbg.h"
#include "data/spatialgrid.h"

struct CT_SPB {
  void *value;
  CT_SPB *next;
  float key[4];
};

static size_t find_cell1d(const CT_SpatialGrid *bin, const float *p) {
  return (size_t)((p[0] - bin->offsets[0]) * bin->invStrides[0]);
}

static size_t find_cell2d(const CT_SpatialGrid *bin, const float *p) {
  return (size_t)((p[1] - bin->offsets[1]) * bin->invStrides[1]) +
         find_cell1d(bin, p);
}

static size_t find_cell3d(const CT_SpatialGrid *bin, const float *p) {
  return (size_t)((p[2] - bin->offsets[2]) * bin->invStrides[2]) +
         find_cell2d(bin, p);
}

CT_EXPORT int ct_spgrid_init(CT_SpatialGrid *bin, float *start, float *end,
                             size_t *strides, size_t dims, size_t poolSize) {
  size_t numCells = strides[0];
  for (size_t i = 0; i < dims; i++) {
    bin->widths[i]     = end[i] - start[i];
    bin->offsets[i]    = start[i];
    bin->strides[i]    = strides[i];
    bin->invStrides[i] = (float)strides[i] / bin->widths[i];
    if (i > 0) {
      numCells *= strides[i];
    }
  }
  bin->numCells = numCells;
  bin->find_cell =
      dims == 1 ? find_cell1d : dims == 2 ? find_cell2d : find_cell3d;
  if (!ct_mpool_init(&bin->pool, MAX(numCells, poolSize), sizeof(CT_SPB))) {
    CT_CHECK_MEM(bin->cells = calloc(numCells, sizeof(CT_SPB *)));
    return 0;
  }
fail:
  return 1;
}

CT_EXPORT void ct_spgrid_free(CT_SpatialGrid *bin) {
  free(bin->cells);
  ct_mpool_free_all(&bin->pool);
}

CT_EXPORT int ct_spgrid_insert(CT_SpatialGrid *bin, float *p, void *item) {
  size_t id = bin->find_cell(bin, p);
  CT_CHECK(id >= 0 && id < bin->numCells, "p out of bounds");
  CT_SPB *b = ct_mpool_alloc(&bin->pool);
  CT_CHECK_MEM(b);
  CT_DEBUG("insert @ bin: %zu new: (%p) -> %p", id, b, bin->cells[id]);
  b->value       = item;
  b->next        = bin->cells[id];
  *b->key        = *p;
  bin->cells[id] = b;
  return 0;
fail:
  return 1;
}

CT_EXPORT int ct_spgrid_remove(CT_SpatialGrid *bin, float *p, void *item) {
  size_t id = bin->find_cell(bin, p);
  CT_CHECK(id >= 0 && id < bin->numCells, "x out of bounds");
  CT_SPB *b = bin->cells[id], *prev = NULL;
  while (b) {
    if (b->value == item) {
      CT_DEBUG("remove @ bin: %zu, %p -> (%p) -> %p", id, prev, b, b->next);
      if (prev == NULL) {
        bin->cells[id] = b->next;
      } else {
        prev->next = b->next;
      }
      ct_mpool_free(&bin->pool, b);
      return 0;
    }
    prev = b;
    b    = b->next;
  }
fail:
  return 1;
}

CT_EXPORT int ct_spgrid_update(CT_SpatialGrid *bin, float *p1, float *p2,
                               void *item) {
  size_t id1 = bin->find_cell(bin, p1);
  size_t id2 = bin->find_cell(bin, p2);
  if (id1 != id2) {
    return !ct_spgrid_remove(bin, p1, item) && ct_spgrid_insert(bin, p2, item);
  }
  return 0;
}

CT_EXPORT size_t ct_spgrid_select(CT_SpatialGrid *bin, float *p, float eps,
                                  void **results, size_t len) {
  size_t i = 0;
  float x1 = *p - eps;
  float x2 = *p + eps;
  size_t s = bin->find_cell(bin, &x1);
  size_t e = bin->find_cell(bin, &x2);
  s        = MAX(s, 0);
  e        = MIN(e, bin->strides[0] - 1);
  while (s <= e) {
    CT_SPB *b = bin->cells[s];
    while (b) {
      if (*b->key >= x1 && *b->key <= x2) {
        CT_DEBUG("sel @ %zu: %p = %p / %p -> %p", s, &results[i], b->value, b,
                 b->next);
        results[i] = b->value;
        i++;
        if (i == len) {
          return i;
        }
      }
      b = b->next;
    }
    s++;
  }
  return i;
}
