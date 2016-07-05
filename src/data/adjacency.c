#include <stdio.h>
#include <stdlib.h>

#include "data/adjacency.h"
#include "math/math.h"

ct_inline size_t cell_index(const CT_AdjMatrix *mat, const size_t x,
                            const size_t y) {
  return x * mat->stride + (y >> 3);
}

ct_export int ct_adjmat_init(CT_AdjMatrix *mat, size_t width) {
  width         = ct_ceil_multiple_pow2(width, 8);
  mat->stride   = width >> 3;
  mat->numBytes = width * mat->stride;
  mat->bits     = calloc(mat->numBytes, 1);
  return mat->bits == NULL;
}

ct_export void ct_adjmat_free(CT_AdjMatrix *mat) {
  free(mat->bits);
}

ct_export int ct_adjmat_join(CT_AdjMatrix *mat, const size_t x,
                             const size_t y) {
  CT_CHECK(x < mat->stride << 3, "x out of bounds");
  CT_CHECK(y < mat->stride << 3, "y out of bounds");
  const size_t id1 = cell_index(mat, x, y);
  const size_t id2 = cell_index(mat, y, x);
  CT_DEBUG("connect: %zu -> %zu, idx: %zu,%zu, bits: %zu,%zu", x, y, id1, id2,
           y & 7, x & 7);
  mat->bits[id1] |= (1 << (y & 7));
  mat->bits[id2] |= (1 << (x & 7));
  return 0;
fail:
  return 1;
}

ct_export int ct_adjmat_disjoin(CT_AdjMatrix *mat, const size_t x,
                                const size_t y) {
  CT_CHECK(x < mat->stride << 3, "x out of bounds");
  CT_CHECK(y < mat->stride << 3, "y out of bounds");
  const size_t id1 = cell_index(mat, x, y);
  const size_t id2 = cell_index(mat, y, x);
  CT_DEBUG("disconnect: %zu -> %zu, idx: %zu,%zu, bits: %zu,%zu", x, y, id1,
           id2, y & 7, x & 7);
  mat->bits[id1] &= ~(1 << (y & 7));
  mat->bits[id2] &= ~(1 << (x & 7));
  return 0;
fail:
  return 1;
}

ct_export int ct_adjmat_is_join(CT_AdjMatrix *mat, const size_t x,
                                const size_t y) {
  CT_CHECK(x < mat->stride << 3, "x out of bounds");
  CT_CHECK(y < mat->stride << 3, "y out of bounds");
  const size_t id1 = cell_index(mat, x, y);
  const size_t id2 = cell_index(mat, y, x);
  return mat->bits[id1] & (1 << (y & 7)) || mat->bits[id2] & (1 << (x & 7));
fail:
  return -1;
}

ct_export void ct_adjmat_trace(const CT_AdjMatrix *mat) {
  uint8_t *ptr = mat->bits;
  for (size_t y = 0, w = mat->stride << 3; y < w; y++) {
    fprintf(stderr, "%04zx: ", y);
    for (size_t x = 0; x < mat->stride; x++) {
      if (*ptr) {
        fprintf(stderr, "%02x ", *ptr);
      } else {
        fputs(".. ", stderr);
      }
      ptr++;
    }
    fputs("\n", stderr);
  }
}

ct_export int ct_adjmat_iterate(const CT_AdjMatrix *mat, CT_AdjMatIterator iter,
                                void *state) {
  uint8_t *ptr = mat->bits;
  for (size_t y = 0, w = mat->stride << 3; y < w; y++) {
    for (size_t x = 0; x <= y >> 3; x++) {
      uint8_t cell = ptr[x];
      if (cell) {
        size_t bmax = 8 - ct_clz8(cell);
        for (size_t b = 0, bv = 1, xx = x << 3; b < bmax && xx <= y;
             b++, bv <<= 1, xx++) {
          if (cell & bv) {
            int res = iter(xx, y, state);
            if (res) return res;
          }
        }
      }
    }
    ptr += mat->stride;
  }
  return 0;
}
