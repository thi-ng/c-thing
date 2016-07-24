#include "sim/verlet.h"
#include "math/vec.h"

static void accelerate2d(CT_Verlet *v) {
  float *pos     = v->pos;
  const float gx = v->gravity[0] * v->dt;
  const float gy = v->gravity[1] * v->dt;
  for (size_t num = v->numP << 1, i = 0; i < num; i += 2) {
    const float x = pos[i] + gx;
    const float y = pos[i + 1] + gy;
    ct_spgrid_update(&v->accel, &pos[i], FVEC(x, y), (void *)i);
    pos[i]     = x;
    pos[i + 1] = y;
  }
}

static void accelerate3d(CT_Verlet *v) {
  float *pos     = v->pos;
  const float gx = v->gravity[0] * v->dt;
  const float gy = v->gravity[1] * v->dt;
  const float gz = v->gravity[2] * v->dt;
  for (size_t num = v->numP << 2, i = 0; i < num; i += 4) {
    const float x = pos[i] + gx;
    const float y = pos[i + 1] + gy;
    const float z = pos[i + 2] + gz;
    ct_spgrid_update(&v->accel, &pos[i], FVEC(x, y, z), (void *)i);
    pos[i]     = x;
    pos[i + 1] = y;
    pos[i + 2] = z;
  }
}

static void collide2d(CT_Verlet *v, size_t preserve) {
  static size_t sel[0x100];
  float *pos            = v->pos;
  float *prev           = v->prev;
  const float repulsion = 0.5 * v->repulsion;
  const float maxf      = v->maxForce;
  for (size_t num = v->numP << 1, i = 0; i < num; i += 2) {
    CT_Vec2f *p = (CT_Vec2f *)&pos[i];
    CT_Vec2f pvel;
    ct_sub2fv(p, (CT_Vec2f *)&prev[i], &pvel);
    const float pr   = v->radius[i >> 1];
    const size_t res = ct_spgrid_select2d(
        &v->accel, &pos[i], FVEC(pr * 2, pr * 2), (void **)&sel, 0x100);
    if (res) {
      for (size_t k = 0; k < res; k++) {
        const size_t id = (size_t)sel[k];
        if (id < num && id != i) {
          CT_Vec2f *q = (CT_Vec2f *)&pos[id];
          CT_Vec2f delta;
          ct_sub2fv(p, q, &delta);
          float d    = ct_magsq2f(&delta);
          float mind = pr + v->radius[id >> 1];
          if (d < mind * mind) {
            float l = sqrtf(d) + EPS;
            l       = (l - mind) / l * repulsion;
            CT_Vec2f delta_scaled, np, nq;
            ct_mul2fn(&delta, l, &delta_scaled);
            ct_limit2f_imm(&delta_scaled, maxf);
            ct_sub2fv(p, &delta_scaled, &np);
            ct_spgrid_update(&v->accel, &pos[i], (float *)&np, (void *)i);
            *p = np;
            ct_add2fv(q, &delta_scaled, &nq);
            if (preserve) {
              CT_Vec2f qvel;
              ct_sub2fv(q, (CT_Vec2f *)&prev[id], &qvel);
              l        = v->friction / d;
              float f1 = ct_dot2fv(&delta, &pvel) * l;
              float f2 = ct_dot2fv(&delta, &qvel) * l;
              float fx = f2 * delta.x - f1 * delta.x;
              float fy = f2 * delta.y - f1 * delta.y;
              ct_add2fxy_imm(&pvel, fx, fy);
              ct_sub2fxy_imm(&qvel, fx, fy);
              ct_sub2fv(&nq, &qvel, (CT_Vec2f *)&prev[id]);
            }
            ct_spgrid_update(&v->accel, &pos[id], (float *)&nq, (void *)id);
            *q = nq;
          }
        }
      }
      if (preserve) {
        ct_sub2fv(p, &pvel, (CT_Vec2f *)&prev[i]);
      }
    }
  }
}

static void collide3d(CT_Verlet *v, size_t preserve) {
  static size_t sel[0x100];
  float *pos            = v->pos;
  float *prev           = v->prev;
  const float repulsion = 0.5 * v->repulsion;
  const float maxf      = v->maxForce;
  for (size_t num = v->numP << 2, i = 0; i < num; i += 4) {
    CT_Vec3f *p = (CT_Vec3f *)&pos[i];
    CT_Vec3f pvel;
    ct_sub3fv(p, (CT_Vec3f *)&prev[i], &pvel);
    const float pr   = v->radius[i >> 2];
    const size_t res = ct_spgrid_select3d(
        &v->accel, &pos[i], FVEC(pr * 2, pr * 2, pr * 2), (void **)&sel, 0x100);
    if (res) {
      for (size_t k = 0; k < res; k++) {
        const size_t id = (size_t)sel[k];
        if (id < num && id != i) {
          CT_Vec3f *q = (CT_Vec3f *)&pos[id];
          CT_Vec3f delta;
          ct_sub3fv(p, q, &delta);
          float d    = ct_magsq3f(&delta);
          float mind = pr + v->radius[id >> 1];
          if (d < mind * mind) {
            float l = sqrtf(d) + EPS;
            l       = (l - mind) / l * repulsion;
            CT_Vec3f delta_scaled, np, nq;
            ct_mul3fn(&delta, l, &delta_scaled);
            ct_limit3f_imm(&delta_scaled, maxf);
            ct_sub3fv(p, &delta_scaled, &np);
            ct_spgrid_update(&v->accel, &pos[i], (float *)&np, (void *)i);
            *p = np;
            ct_add3fv(q, &delta_scaled, &nq);
            if (preserve) {
              CT_Vec3f qvel;
              ct_sub3fv(q, (CT_Vec3f *)&prev[id], &qvel);
              l        = v->friction / d;
              float f1 = ct_dot3fv(&delta, &pvel) * l;
              float f2 = ct_dot3fv(&delta, &qvel) * l;
              float fx = f2 * delta.x - f1 * delta.x;
              float fy = f2 * delta.y - f1 * delta.y;
              float fz = f2 * delta.z - f1 * delta.z;
              ct_add3fxyz_imm(&pvel, fx, fy, fz);
              ct_sub3fxyz_imm(&qvel, fx, fy, fz);
              ct_sub3fv(&nq, &qvel, (CT_Vec3f *)&prev[id]);
            }
            ct_spgrid_update(&v->accel, &pos[id], (float *)&nq, (void *)id);
            *q = nq;
          }
        }
      }
      if (preserve) {
        ct_sub3fv(p, &pvel, (CT_Vec3f *)&prev[i]);
      }
    }
  }
}

// TODO swap prev & pos ptr to avoid copying

static void inertia2d(CT_Verlet *v) {
  float *pos  = v->pos;
  float *prev = v->prev;
  for (size_t num = v->numP << 1, i = 0; i < num; i += 2) {
    const float x = pos[i] * 2.0f - prev[i];
    const float y = pos[i + 1] * 2.0f - prev[i + 1];
    prev[i]       = pos[i];
    prev[i + 1]   = pos[i + 1];
    ct_spgrid_update(&v->accel, &pos[i], FVEC(x, y), (void *)i);
    pos[i]     = x;
    pos[i + 1] = y;
  }
}

static void inertia3d(CT_Verlet *v) {
  float *pos  = v->pos;
  float *prev = v->prev;
  for (size_t num = v->numP << 2, i = 0; i < num; i += 4) {
    const float x = pos[i] * 2.0f - prev[i];
    const float y = pos[i + 1] * 2.0f - prev[i + 1];
    const float z = pos[i + 2] * 2.0f - prev[i + 2];
    prev[i]       = pos[i];
    prev[i + 1]   = pos[i + 1];
    prev[i + 2]   = pos[i + 2];
    ct_spgrid_update(&v->accel, &pos[i], FVEC(x, y, z), (void *)i);
    pos[i]     = x;
    pos[i + 1] = y;
    pos[i + 2] = z;
  }
}

static void border2d(CT_Verlet *v) {
  float *pos      = v->pos;
  const float bx1 = v->bounds[0];
  const float by1 = v->bounds[1];
  const float bx2 = v->bounds[2];
  const float by2 = v->bounds[3];
  for (size_t num = v->numP << 1, i = 0; i < num; i += 2) {
    if (pos[i] < bx1) {
      ct_spgrid_update(&v->accel, &pos[i], FVEC(bx1, pos[i + 1]), (void *)i);
      pos[i] = bx1;
    } else if (pos[i] > bx2) {
      ct_spgrid_update(&v->accel, &pos[i], FVEC(bx2, pos[i + 1]), (void *)i);
      pos[i] = bx2;
    }
    if (pos[i + 1] < by1) {
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], by1), (void *)i);
      pos[i + 1] = by1;
    } else if (pos[i + 1] > by2) {
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], by2), (void *)i);
      pos[i + 1] = by2;
    }
  }
}

static void border3d(CT_Verlet *v) {
  float *pos      = v->pos;
  const float bx1 = v->bounds[0];
  const float by1 = v->bounds[1];
  const float bz1 = v->bounds[2];
  const float bx2 = v->bounds[3];
  const float by2 = v->bounds[4];
  const float bz2 = v->bounds[5];
  for (size_t num = v->numP << 2, i = 0; i < num; i += 4) {
    if (pos[i] < bx1) {
      ct_spgrid_update(&v->accel, &pos[i], FVEC(bx1, pos[i + 1], pos[i + 2]),
                       (void *)i);
      pos[i] = bx1;
    } else if (pos[i] > bx2) {
      ct_spgrid_update(&v->accel, &pos[i], FVEC(bx2, pos[i + 1], pos[i + 2]),
                       (void *)i);
      pos[i] = bx2;
    }
    if (pos[i + 1] < by1) {
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], by1, pos[i + 2]),
                       (void *)i);
      pos[i + 1] = by1;
    } else if (pos[i + 1] > by2) {
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], by2, pos[i + 2]),
                       (void *)i);
      pos[i + 1] = by2;
    }
    if (pos[i + 2] < by1) {
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], pos[i + 1], bz1),
                       (void *)i);
      pos[i + 2] = bz1;
    } else if (pos[i + 2] > by2) {
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], pos[i + 1], bz2),
                       (void *)i);
      pos[i + 2] = bz2;
    }
  }
}

static void border2d_impulse(CT_Verlet *v) {
  float *pos           = v->pos;
  float *prev          = v->prev;
  const float bx1      = v->bounds[0];
  const float by1      = v->bounds[1];
  const float bx2      = v->bounds[2];
  const float by2      = v->bounds[3];
  const float friction = v->friction;
  for (size_t num = v->numP << 1, i = 0; i < num; i += 2) {
    float p = pos[i];
    if (p < bx1) {
      prev[i] = bx1 - (prev[i] - p) * friction;
      ct_spgrid_update(&v->accel, &pos[i], FVEC(bx1, pos[i + 1]), (void *)i);
      pos[i] = bx1;
    } else if (p > bx2) {
      prev[i] = bx2 - (prev[i] - p) * friction;
      ct_spgrid_update(&v->accel, &pos[i], FVEC(bx2, pos[i + 1]), (void *)i);
      pos[i] = bx2;
    }
    p = pos[i + 1];
    if (p < by1) {
      prev[i + 1] = by1 - (prev[i + 1] - p) * friction;
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], by1), (void *)i);
      pos[i + 1] = by1;
    } else if (p > by2) {
      prev[i + 1] = by2 - (prev[i + 1] - p) * friction;
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], by2), (void *)i);
      pos[i + 1] = by2;
    }
  }
}

static void border3d_impulse(CT_Verlet *v) {
  float *pos           = v->pos;
  float *prev          = v->prev;
  const float bx1      = v->bounds[0];
  const float by1      = v->bounds[1];
  const float bz1      = v->bounds[2];
  const float bx2      = v->bounds[3];
  const float by2      = v->bounds[4];
  const float bz2      = v->bounds[5];
  const float friction = v->friction;
  for (size_t num = v->numP << 2, i = 0; i < num; i += 4) {
    float p = pos[i];
    if (p < bx1) {
      prev[i] = bx1 - (prev[i] - p) * friction;
      ct_spgrid_update(&v->accel, &pos[i], FVEC(bx1, pos[i + 1], pos[i + 2]),
                       (void *)i);
      pos[i] = bx1;
    } else if (p > bx2) {
      prev[i] = bx2 - (prev[i] - p) * friction;
      ct_spgrid_update(&v->accel, &pos[i], FVEC(bx2, pos[i + 1], pos[i + 2]),
                       (void *)i);
      pos[i] = bx2;
    }
    p = pos[i + 1];
    if (p < by1) {
      prev[i + 1] = by1 - (prev[i + 1] - p) * friction;
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], by1, pos[i + 2]),
                       (void *)i);
      pos[i + 1] = by1;
    } else if (p > by2) {
      prev[i + 1] = by2 - (prev[i + 1] - p) * friction;
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], by2, pos[i + 2]),
                       (void *)i);
      pos[i + 1] = by2;
    }
    p = pos[i + 2];
    if (p < bz1) {
      prev[i + 2] = bz1 - (prev[i + 2] - p) * friction;
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], pos[i + 1], bz1),
                       (void *)i);
      pos[i + 2] = bz1;
    } else if (p > bz2) {
      prev[i + 2] = bz2 - (prev[i + 2] - p) * friction;
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], pos[i + 1], bz2),
                       (void *)i);
      pos[i + 2] = bz2;
    }
  }
}

static void springs2d(CT_Verlet *v) {
  float *pos           = v->pos;
  CT_VPSpring *springs = v->springs;
  const float maxf     = v->maxForce;
  for (size_t num = v->numS, i = 0; i < num; i++) {
    CT_VPSpring *s = &springs[i];
    CT_Vec2f *a    = (CT_Vec2f *)&pos[s->a];
    CT_Vec2f *b    = (CT_Vec2f *)&pos[s->b];
    CT_Vec2f delta;
    ct_sub2fv(a, b, &delta);
    float d = ct_mag2f(&delta) + EPS;
    ct_mul2fn_imm(&delta, (d - s->restLen) / d * s->strength);
    ct_limit2f_imm(&delta, maxf);
    float dx = a->x - delta.x;
    float dy = a->y - delta.y;
    ct_spgrid_update(&v->accel, (float *)a, FVEC(dx, dy), (void *)s->a);
    a->x = dx;
    a->y = dy;
    dx   = b->x + delta.x;
    dy   = b->y + delta.y;
    ct_spgrid_update(&v->accel, (float *)b, FVEC(dx, dy), (void *)s->b);
    b->x = dx;
    b->y = dy;
  }
}

static void springs3d(CT_Verlet *v) {
  float *pos           = v->pos;
  CT_VPSpring *springs = v->springs;
  const float maxf     = v->maxForce;
  for (size_t num = v->numS, i = 0; i < num; i++) {
    CT_VPSpring *s = &springs[i];
    CT_Vec3f *a    = (CT_Vec3f *)&pos[s->a];
    CT_Vec3f *b    = (CT_Vec3f *)&pos[s->b];
    CT_Vec3f delta;
    ct_sub3fv(a, b, &delta);
    float d = ct_mag3f(&delta) + EPS;
    ct_mul3fn_imm(&delta, (d - s->restLen) / d * s->strength);
    ct_limit3f_imm(&delta, maxf);
    float dx = a->x - delta.x;
    float dy = a->y - delta.y;
    float dz = a->z - delta.y;
    ct_spgrid_update(&v->accel, (float *)a, FVEC(dx, dy, dz), (void *)s->a);
    a->x = dx;
    a->y = dy;
    a->z = dz;
    dx   = b->x + delta.x;
    dy   = b->y + delta.y;
    dz   = b->z + delta.z;
    ct_spgrid_update(&v->accel, (float *)b, FVEC(dx, dy, dz), (void *)s->b);
    b->x = dx;
    b->y = dy;
    b->z = dz;
  }
}

ct_export int ct_verlet_init2d(CT_Verlet *v,
                               size_t maxP,
                               size_t maxS,
                               size_t *grid) {
  maxP       = ct_ceil_multiple_pow2(maxP, 4);
  float *buf = calloc((3 * 2 + 1) * maxP, sizeof(float));
  CT_CHECK_MEM(buf);
  v->pos               = buf;
  v->prev              = buf + maxP * 2;
  v->force             = buf + maxP * 4;
  v->radius            = buf + maxP * 6;
  CT_VPSpring *springs = calloc(maxS, sizeof(CT_VPSpring));
  CT_CHECK_MEM(springs);
  v->springs = springs;
  v->strideP = maxP;
  v->strideS = maxS;
  v->numP    = 0;
  v->numS    = 0;
  v->dt      = 1.0f / v->iter;
  v->dt *= v->dt;
  CT_Vec2f a, b;
  ct_sub2fn((CT_Vec2f *)v->bounds, v->maxForce * 10, &a);
  ct_add2fn((CT_Vec2f *)&v->bounds[2], v->maxForce * 10, &b);
  return ct_spgrid_init(&v->accel, (float *)&a, (float *)&b, grid, 2, 0x100);
fail:
  return 1;
}

ct_export int ct_verlet_init3d(CT_Verlet *v,
                               size_t maxP,
                               size_t maxS,
                               size_t *grid) {
  maxP       = ct_ceil_multiple_pow2(maxP, 4);
  float *buf = calloc((3 * 4 + 1) * maxP, sizeof(float));
  CT_CHECK_MEM(buf);
  v->pos               = buf;
  v->prev              = buf + maxP * 4;
  v->force             = buf + maxP * 8;
  v->radius            = buf + maxP * 12;
  CT_VPSpring *springs = calloc(maxS, sizeof(CT_VPSpring));
  CT_CHECK_MEM(springs);
  v->springs = springs;
  v->strideP = maxP;
  v->strideS = maxS;
  v->numP    = 0;
  v->numS    = 0;
  v->dt      = 1.0f / v->iter;
  v->dt *= v->dt;
  CT_Vec3f a, b;
  ct_sub3fn((CT_Vec3f *)v->bounds, v->maxForce * 10, &a);
  ct_add3fn((CT_Vec3f *)&v->bounds[3], v->maxForce * 10, &b);
  return ct_spgrid_init(&v->accel, (float *)&a, (float *)&b, grid, 3, 0x100);
fail:
  return 1;
}

ct_export void ct_verlet_free(CT_Verlet *v) {
  free(v->pos);
  ct_spgrid_free(&v->accel);
}

ct_export void ct_verlet_update2d(CT_Verlet *v) {
  for (size_t i = 0; i < v->iter; i++) {
    accelerate2d(v);
    springs2d(v);
    collide2d(v, 0);
    border2d(v);
    inertia2d(v);
    collide2d(v, 1);
    border2d_impulse(v);
  }
}

ct_export void ct_verlet_update3d(CT_Verlet *v) {
  for (size_t i = 0; i < v->iter; i++) {
    accelerate3d(v);
    springs3d(v);
    collide3d(v, 0);
    border3d(v);
    inertia3d(v);
    collide3d(v, 1);
    border3d_impulse(v);
  }
}

ct_export void ct_verlet_set2f(CT_Verlet *v,
                               size_t i,
                               const float *pos,
                               float radius) {
  v->radius[i] = radius;
  i <<= 1;
  v->pos[i] = v->prev[i] = pos[0];
  v->pos[i + 1] = v->prev[i + 1] = pos[1];
  ct_spgrid_insert(&v->accel, pos, (void *)i);
}

ct_export void ct_verlet_set3f(CT_Verlet *v,
                               size_t i,
                               const float *pos,
                               float radius) {
  v->radius[i] = radius;
  i <<= 2;
  v->pos[i] = v->prev[i] = pos[0];
  v->pos[i + 1] = v->prev[i + 1] = pos[1];
  v->pos[i + 2] = v->prev[i + 2] = pos[2];
  ct_spgrid_insert(&v->accel, pos, (void *)i);
}

ct_export void ct_verlet_set_spring2d(CT_Verlet *v,
                                      size_t i,
                                      size_t a,
                                      size_t b,
                                      float len,
                                      float strength) {
  CT_VPSpring *s = &v->springs[i];
  s->a           = a << 1;
  s->b           = b << 1;
  s->restLen     = len;
  s->strength    = strength;
  CT_DEBUG("add spring2d: %zu, a: %zu, b: %zu, rl: %f, str: %f", i, a, b, len,
           strength);
}

ct_export void ct_verlet_set_spring3d(CT_Verlet *v,
                                      size_t i,
                                      size_t a,
                                      size_t b,
                                      float len,
                                      float strength) {
  CT_VPSpring *s = &v->springs[i];
  s->a           = a << 2;
  s->b           = b << 2;
  s->restLen     = len;
  s->strength    = strength;
  CT_DEBUG("add spring3d: %zu, a: %zu, b: %zu, rl: %f, str: %f", i, a, b, len,
           strength);
}
