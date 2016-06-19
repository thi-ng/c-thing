#pragma once

#include <stddef.h>

#include "data/spatialgrid.h"

typedef struct {
  float *pos, *prev, *force, *radius;
  CT_SpatialGrid accel;
  size_t num;
  size_t stride;
  size_t iter;
  float dt;
  float friction;
  float minD;
  float gravity[3];
  float bounds[8];
} CT_Verlet;

int ct_verlet_init(CT_Verlet *v, size_t max, float margin, size_t *grid);
void ct_verlet_free(CT_Verlet *v);
void ct_verlet_update2d(CT_Verlet *v);
void ct_verlet_trace(CT_Verlet *v);
void ct_verlet_set2f(CT_Verlet *v, size_t i, const float *pos);

CT_EXPORT ct_inline void ct_verlet_pos2f(const CT_Verlet *v, size_t i,
                                         float *out) {
  i <<= 1;
  out[0] = v->pos[i];
  out[1] = v->pos[i + 1];
}
