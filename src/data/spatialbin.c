#include <stddef.h>
#include <stdlib.h>

#include "common/dbg.h"
#include "data/spatialbin.h"

struct CT_SPB {
  void *value;
  CT_SPB *next;
};

static size_t find_bin(const CT_SpatialBin *bin, float x) {
  return (size_t)((x - bin->offset) * bin->invBinWidth);
}

int ct_spbin_init(CT_SpatialBin *bin, float x1, float x2, size_t numBins,
                  size_t poolSize) {
  if (!ct_mpool_init(&bin->pool, MAX(numBins, poolSize), sizeof(CT_SPB))) {
    CT_CHECK_MEM(bin->bins = calloc(numBins, sizeof(CT_SPB *)));
    bin->numBins     = numBins;
    bin->offset      = x1;
    bin->totalWidth  = x2 - x1;
    bin->invBinWidth = (float)numBins / bin->totalWidth;
    CT_DEBUG("invBinwidth: %f", bin->invBinWidth);
    return 0;
  }
fail:
  return 1;
}

void ct_spbin_free(CT_SpatialBin *bin) {
  ct_mpool_free_all(&bin->pool);
}

int ct_spbin_insert(CT_SpatialBin *bin, float x, void *item) {
  size_t id = find_bin(bin, x);
  CT_CHECK(id >= 0 && id < bin->numBins, "x out of bounds");
  CT_SPB *b = ct_mpool_alloc(&bin->pool);
  CT_CHECK_MEM(b);
  CT_DEBUG("insert @ bin: %zu new: (%p) -> %p", id, b, bin->bins[id]);
  b->value      = item;
  b->next       = bin->bins[id];
  bin->bins[id] = b;
  return 0;
fail:
  return 1;
}

int ct_spbin_remove(CT_SpatialBin *bin, float x, void *item) {
  size_t id = find_bin(bin, x);
  CT_CHECK(id >= 0 && id < bin->numBins, "x out of bounds");
  CT_SPB *b = bin->bins[id], *p = NULL;
  while (b) {
    if (b->value == item) {
      CT_DEBUG("remove @ bin: %zu, %p -> (%p) -> %p", id, p, b, b->next);
      if (p == NULL) {
        bin->bins[id] = b->next;
      } else {
        p->next = b->next;
      }
      ct_mpool_free(&bin->pool, b);
      return 0;
    }
    p = b;
    b = b->next;
  }
fail:
  return 1;
}

size_t ct_spbin_select(CT_SpatialBin *bin, float x, float eps, void **results,
                       size_t len) {
  size_t i = 0;
  size_t s = find_bin(bin, x - eps);
  size_t e = find_bin(bin, x + eps);
  s        = MAX(s, 0);
  e        = MIN(e, bin->numBins - 1);
  while (s <= e) {
    CT_DEBUG("selecting bin: %zu", s);
    CT_SPB *b = bin->bins[s];
    while (b) {
      CT_DEBUG("sel: %p = %p / %p -> %p", &results[i], b->value, b, b->next);
      results[i] = b->value;
      b          = b->next;
      i++;
      if (i == len) {
        return i;
      }
    }
    s++;
  }
  return i;
}
