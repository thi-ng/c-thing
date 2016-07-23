#pragma once

#include "cthing.h"

#include "data/spatialgrid.h"

CT_BEGIN_DECLS

typedef struct {
  size_t a;
  size_t b;
  float restLen;
  float strength;
} CT_VPSpring;

typedef struct {
  float *pos, *prev, *force, *radius;
  CT_VPSpring *springs;
  CT_SpatialGrid accel;
  size_t numP;
  size_t numS;
  size_t strideP;
  size_t strideS;
  size_t iter;
  float dt;
  float friction;
  float repulsion;
  float maxForce;
  float gravity[4];
  float bounds[8];
} CT_Verlet;

int ct_verlet_init2d(CT_Verlet *v, size_t maxP, size_t maxS, size_t *grid);
int ct_verlet_init3d(CT_Verlet *v, size_t maxP, size_t maxS, size_t *grid);
void ct_verlet_free(CT_Verlet *v);
void ct_verlet_update2d(CT_Verlet *v);
void ct_verlet_update3d(CT_Verlet *v);
void ct_verlet_trace(CT_Verlet *v);
void ct_verlet_set2f(CT_Verlet *v, size_t i, const float *pos, float radius);
void ct_verlet_set3f(CT_Verlet *v, size_t i, const float *pos, float radius);
void ct_verlet_set_spring2d(CT_Verlet *v,
                            size_t i,
                            size_t a,
                            size_t b,
                            float len,
                            float strength);
void ct_verlet_set_spring3d(CT_Verlet *v,
                            size_t i,
                            size_t a,
                            size_t b,
                            float len,
                            float strength);

ct_export ct_inline void ct_verlet_pos2f(const CT_Verlet *v,
                                         size_t i,
                                         float *out) {
  out[2] = v->radius[i];
  i <<= 1;
  out[0] = v->pos[i];
  out[1] = v->pos[i + 1];
}

ct_export ct_inline void ct_verlet_pos3f(const CT_Verlet *v,
                                         size_t i,
                                         float *out) {
  out[2] = v->radius[i];
  i <<= 2;
  out[0] = v->pos[i];
  out[1] = v->pos[i + 1];
  out[2] = v->pos[i + 2];
}

CT_END_DECLS
