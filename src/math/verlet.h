#pragma once

#include <stddef.h>

#include "math/vec.h"

typedef struct {
  float *pos, *prev, *force, *mass;
  size_t num;
  float timeStep;
  float friction;
  float minD;
  float gravity[2];
  float bounds[4];
} CT_Verlet2f;

int ct_verlet_init(CT_Verlet2f *verlet, size_t num);
void ct_verlet_update2d(CT_Verlet2f *verlet);
void ct_verlet_trace(CT_Verlet2f *v);

CT_EXPORT ct_inline void ct_verlet_set2f(CT_Verlet2f *verlet, size_t i,
                                         const float *pos, float m) {
  verlet->pos[i] = verlet->prev[i] = pos[0];
  verlet->pos[i + verlet->num] = verlet->prev[i + verlet->num] = pos[1];
  verlet->mass[i]                                              = 1.0 / m;
}

CT_EXPORT ct_inline void ct_verlet_pos2f(const CT_Verlet2f *verlet, size_t i,
                                         float *out) {
  out[0] = verlet->pos[i];
  out[1] = verlet->pos[i + verlet->num];
}
