#include <stdio.h>
#include <string.h>

#include "math/math.h"
#include "sim/ca.h"

int ct_carule1d_init(CT_CARule1D *r) {
  CT_CHECK(r->kernelWidth < 4, "kernel width");
  CT_CHECK(r->numStates > 1, "num states");
  uint8_t b = 1;
  for (uint8_t i = r->kernelWidth; i > 0; i--) {
    b <<= 2;
  }
  r->maxBitValue = b;
  return 0;
fail:
  return 1;
}

void ct_carule1d_evolve(const CT_CARule1D *rule, CT_CAMatrix *mat) {
  uint8_t *curr             = mat->matrix;
  uint8_t *next             = mat->swap;
  const int kw              = rule->kernelWidth;
  const size_t maxState     = rule->numStates - 1;
  const size_t kmax         = rule->maxBitValue;
  const size_t ruleMask     = rule->rule;
  const size_t isTiling     = rule->flags & CT_CA_TILING;
  const size_t isAutoExpire = rule->flags & CT_CA_AUTO_EXPIRE;
  for (int i = 0, len = mat->width, l2 = len - kw; i < len; i++) {
    size_t sum = 0;
    size_t k   = kmax;
    int idx    = i - kw;
    if (i >= kw && i < l2) {
      for (int j = -kw; j <= kw; j++, k >>= 1) {
        if (curr[idx++]) {
          sum += k;
        }
      }
    } else {
      if (isTiling) {
        idx %= len;
        if (idx < 0) {
          idx += len;
        }
      }
      for (int j = -kw; j <= kw; j++, k >>= 1, idx++) {
        if (isTiling) {
          idx %= len;
          if (curr[idx]) {
            sum += k;
          }
        } else if (idx >= 0 && idx < len && curr[idx]) {
          sum += k;
        }
      }
    }
    uint8_t c      = curr[i] + 1;
    size_t isAlive = ((ruleMask & (1UL << sum)) > 0);
    if (isAutoExpire) {
      c = isAlive ? (c % rule->numStates) : 0;
    } else {
      c = isAlive ? (c < maxState ? c : maxState) : 0;
    }
    next[i] = c;
  }
  mat->matrix = next;
  mat->swap   = curr;
  mat->gen++;
}

int ct_carule2d_init(CT_CARule2D *r) {
  CT_CHECK(r->numStates > 1, "num states");
  const int kw     = r->kernelWidth;
  const int kw2    = 2 * kw + 1;
  r->kernelOffsets = (size_t *)calloc(kw2 * kw2 - 1, sizeof(size_t));
  CT_CHECK_MEM(r->kernelOffsets);
  for (int y = -kw, i = 0; y <= kw; y++) {
    for (int x = -kw; x <= kw; x++) {
      if ((y != 0) || (x != 0)) {
        r->kernelOffsets[i++] = (uint8_t)x << 8 | (uint8_t)y;
      }
    }
  }
fail:
  return r->kernelOffsets == NULL;
}

void ct_carule2d_free(CT_CARule2D *r) {
  free(r->kernelOffsets);
}

void ct_carule2d_evolve(const CT_CARule2D *rule, CT_CAMatrix *mat) {
  uint8_t *curr             = mat->matrix;
  uint8_t *next             = mat->swap;
  uint8_t maxk              = 2 * rule->kernelWidth + 1;
  maxk                      = maxk * maxk - 2;
  const size_t *koff        = rule->kernelOffsets;
  const size_t birth        = rule->birthRule;
  const size_t survival     = rule->survivalRule;
  const size_t maxState     = rule->numStates - 1;
  const size_t isTiling     = rule->flags & CT_CA_TILING;
  const size_t isAutoExpire = rule->flags & CT_CA_AUTO_EXPIRE;
  const size_t mw           = mat->width;
  const size_t mh           = mat->height;
  size_t x1 = 0, y1 = 0, x2 = mw, y2 = mh;
  if (!isTiling) {
    x1 = y1 = rule->kernelWidth;
    x2 -= x1;
    y2 -= y1;
  }
  for (size_t y = y1; y < y2; y++) {
    size_t idx = y * mw + x1;
    for (size_t x = x1; x < x2; x++) {
      size_t sum = 1;
      for (int k = maxk; k >= 0; k--) {
        size_t off = koff[k];
        int xx     = x + (int8_t)(off >> 8);
        int yy     = y + (int8_t)(off & 0xff);
        xx         = (xx < 0) ? (xx + mw) : ((xx >= mw) ? (xx - mw) : xx);
        yy         = (yy < 0) ? (yy + mh) : ((yy >= mh) ? (yy - mh) : yy);
        if (curr[yy * mw + xx]) {
          sum <<= 1;
        }
      }
      uint8_t c = curr[idx];
      if (c) {
        if (survival & sum) {
          c++;
          c = isAutoExpire ? (c % rule->numStates)
                           : (c < maxState ? c : maxState);
        } else {
          c ^= c;
        }
      } else {
        c = (birth & sum) > 0;
      }
      next[idx++] = c;
    }
  }
  mat->matrix = next;
  mat->swap   = curr;
  mat->gen++;
}

int ct_camatrix_init(CT_CAMatrix *m) {
  uint32_t len = m->width * m->height;
  m->gen       = 0;
  m->matrix    = (uint8_t *)calloc(len, sizeof(uint8_t));
  m->swap      = (uint8_t *)calloc(len, sizeof(uint8_t));
  return m->matrix == NULL || m->swap == NULL;
}

void ct_camatrix_free(CT_CAMatrix *m) {
  free(m->matrix);
  free(m->swap);
}

void ct_camatrix_seed1d(CT_CAMatrix *mat, size_t seed) {
  for (int i = (mat->width <= 64 ? mat->width : 64) - 1; i >= 0; i--) {
    mat->matrix[i] = seed & 1;
    seed >>= 1;
  }
}

void ct_camatrix_seed2d(CT_CAMatrix *mat,
                        size_t x,
                        size_t y,
                        size_t w,
                        size_t h,
                        const char *cells) {
  for (size_t yy = 0; (yy < h) && (y < mat->height); yy++, y++) {
    size_t idx = y * mat->width + x;
    for (size_t xx = 0; (xx < w) && (x < mat->width); xx++, idx++) {
      mat->matrix[idx] = (cells[idx + xx] != ' ') ? 1 : 0;
    }
  }
}

void ct_camatrix_seed_noise(CT_CAMatrix *mat, float prob) {
  size_t num = mat->width * mat->height;
  for (size_t i = 0; i < num; i++) {
    mat->matrix[i] = (ct_rand_normpos() < prob);
  }
}

void ct_camatrix_clear(CT_CAMatrix *mat) {
  size_t len = mat->width * mat->height;
  memset(mat->matrix, 0, len);
  memset(mat->swap, 0, len);
  mat->gen = 0;
}

void ct_camatrix_trace(const CT_CAMatrix *mat) {
  uint8_t *cell = mat->matrix;
  for (size_t y = 0; y < mat->height; y++) {
    for (int x = 0; x < mat->width; x++) {
      putc(*cell++ > 0 ? '#' : ' ', stdout);
    }
    putc('\n', stdout);
  }
}
