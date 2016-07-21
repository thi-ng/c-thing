#pragma once

#include <stddef.h>
#include <stdint.h>

#include "cthing.h"

enum { CT_CA_TILING = 1, CT_CA_AUTO_EXPIRE = 2 };

typedef struct {
  size_t rule;
  uint8_t maxBitValue;
  uint8_t numBits;
  uint8_t numStates;
  uint8_t kernelWidth;
  uint8_t flags;
} CT_CARule1D;

typedef struct {
  size_t *kernelOffsets;
  uint16_t birthRule;
  uint16_t survivalRule;
  uint8_t kernelWidth;
  uint8_t numStates;
  uint8_t flags;
} CT_CARule2D;

typedef struct {
  uint8_t *matrix;
  uint8_t *swap;
  size_t width;
  size_t height;
  size_t gen;
} CT_CAMatrix;

int ct_carule1d_init(CT_CARule1D *rule);
void ct_carule1d_free(CT_CARule1D *rule);
void ct_carule1d_evolve(const CT_CARule1D *rule, CT_CAMatrix *mat);

int ct_carule2d_init(CT_CARule2D *rule);
void ct_carule2d_free(CT_CARule2D *rule);
void ct_carule2d_evolve(const CT_CARule2D *rule, CT_CAMatrix *mat);

int ct_camatrix_init(CT_CAMatrix *mat);
void ct_camatrix_free(CT_CAMatrix *mat);
void ct_camatrix_clear(CT_CAMatrix *mat);
void ct_camatrix_seed1d(CT_CAMatrix *mat, size_t seed);
void ct_camatrix_seed2d(CT_CAMatrix *mat,
                        size_t x,
                        size_t y,
                        size_t w,
                        size_t h,
                        const char *cells);
void ct_camatrix_seed_noise(CT_CAMatrix *mat, float prob);
void ct_camatrix_trace(const CT_CAMatrix *mat);
