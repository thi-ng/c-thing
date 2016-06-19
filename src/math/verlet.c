#include "common/dbg.h"

#include "math/vec.h"
#include "math/verlet.h"

static void accelerate2d(CT_Verlet *v) {
  float *pos     = v->pos;
  const float dt = v->dt;
  const float gx = v->gravity[0] * dt;
  const float gy = v->gravity[1] * dt;
  for (size_t num = v->num << 1, i = 0; i < num; i += 2) {
    const float x = pos[i] + gx;
    const float y = pos[i + 1] + gy;
    ct_spgrid_update(&v->accel, &pos[i], FVEC(x, y), (void *)(i >> 1));
    pos[i]     = x;
    pos[i + 1] = y;
  }
}

static void collide2d(CT_Verlet *v, size_t preserve) {
  static size_t sel[0x100];
  float *pos    = v->pos;
  float *prev   = v->prev;
  float *eps    = FVEC(v->minD, v->minD);
  float mind_sq = v->minD * v->minD;
  for (size_t num = v->num << 1, i = 0; i < num; i += 2) {
    CT_Vec2f *p = (CT_Vec2f *)&pos[i];
    CT_Vec2f pvel;
    ct_sub2fv(p, (CT_Vec2f *)&prev[i], &pvel);
    const size_t res =
        ct_spgrid_select2d(&v->accel, &pos[i], eps, (void **)&sel, 0x100);
    if (res) {
      for (size_t k = 0; k < res; k++) {
        const size_t id = (size_t)sel[k] << 1;
        if (id < num && id != i) {
          CT_Vec2f *q = (CT_Vec2f *)&pos[id];
          CT_Vec2f delta;
          ct_sub2fv(p, q, &delta);
          float d = ct_magsq2f(&delta);
          if (d < mind_sq) {
            float l = sqrtf(d);
            l       = (l - v->minD) / l * 0.5f;
            //l = CLAMP(l, -1, 1);
            CT_Vec2f delta_scaled, np, nq;
            ct_mul2fn(&delta, l, &delta_scaled);
            ct_sub2fv(p, &delta_scaled, &np);
            ct_spgrid_update(&v->accel, &pos[i], (float *)&np,
                             (void *)(i >> 1));
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
            ct_spgrid_update(&v->accel, &pos[id], (float *)&nq,
                             (void *)(id >> 1));
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

static void inertia2d(CT_Verlet *v) {
  float *pos  = v->pos;
  float *prev = v->prev;
  for (size_t num = v->num << 1, i = 0; i < num; i += 2) {
    const float x = pos[i] * 2.0f - prev[i];
    const float y = pos[i + 1] * 2.0f - prev[i + 1];
    prev[i]       = pos[i];
    prev[i + 1]   = pos[i + 1];
    ct_spgrid_update(&v->accel, &pos[i], FVEC(x, y), (void *)(i >> 1));
    pos[i]     = x;
    pos[i + 1] = y;
  }
}

static void border2d(CT_Verlet *v) {
  float *pos      = v->pos;
  const float bx1 = v->bounds[0];
  const float by1 = v->bounds[1];
  const float bx2 = v->bounds[2];
  const float by2 = v->bounds[3];
  for (size_t num = v->num << 1, i = 0; i < num; i += 2) {
    if (pos[i] < bx1) {
      ct_spgrid_update(&v->accel, &pos[i], FVEC(bx1, pos[i + 1]),
                       (void *)(i >> 1));
      pos[i] = bx1;
    } else if (pos[i] > bx2) {
      ct_spgrid_update(&v->accel, &pos[i], FVEC(bx2, pos[i + 1]),
                       (void *)(i >> 1));
      pos[i] = bx2;
    }
    if (pos[i + 1] < by1) {
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], by1), (void *)(i >> 1));
      pos[i + 1] = by1;
    } else if (pos[i + 1] > by2) {
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], by2), (void *)(i >> 1));
      pos[i + 1] = by2;
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
  for (size_t num = v->num << 1, i = 0; i < num; i += 2) {
    float x = pos[i];
    float y = pos[i + 1];
    if (x < bx1) {
      prev[i] = bx1 - (prev[i] - x) * friction;
      ct_spgrid_update(&v->accel, &pos[i], FVEC(bx1, pos[i + 1]),
                       (void *)(i >> 1));
      pos[i] = bx1;
    } else if (x > bx2) {
      prev[i] = bx2 - (prev[i] - x) * friction;
      ct_spgrid_update(&v->accel, &pos[i], FVEC(bx2, pos[i + 1]),
                       (void *)(i >> 1));
      pos[i] = bx2;
    }
    if (y < by1) {
      prev[i + 1] = by1 - (prev[i + 1] - y) * friction;
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], by1), (void *)(i >> 1));
      pos[i + 1] = by1;
    } else if (y > by2) {
      prev[i + 1] = by2 - (prev[i + 1] - y) * friction;
      ct_spgrid_update(&v->accel, &pos[i], FVEC(pos[i], by2), (void *)(i >> 1));
      pos[i + 1] = by2;
    }
  }
}

int ct_verlet_init(CT_Verlet *v, size_t max, float margin, size_t *grid) {
  max        = ct_ceil_multiple_pow2(max, 4);
  float *buf = calloc((3 * 2 + 1) * max, sizeof(float));
  CT_CHECK_MEM(buf);
  v->pos    = buf;
  v->prev   = buf + max * 2;
  v->force  = buf + max * 4;
  v->radius = buf + max * 6;
  v->stride = max;
  v->num    = 0;
  v->dt     = 1.0f / v->iter;
  v->dt *= v->dt;
  CT_Vec2f a, b;
  ct_sub2fn((CT_Vec2f *)v->bounds, margin, &a);
  ct_add2fn((CT_Vec2f *)&v->bounds[2], margin, &b);
  return ct_spgrid_init(&v->accel, (float *)&a, (float *)&b, grid, 2, 0x100);
fail:
  return 1;
}

void ct_verlet_free(CT_Verlet *v) {
  free(v->pos);
  ct_spgrid_free(&v->accel);
}

void ct_verlet_update2d(CT_Verlet *v) {
  for (size_t i = 0; i < v->iter; i++) {
    //attract2f(v);
    accelerate2d(v);
    collide2d(v, 0);
    border2d(v);
    inertia2d(v);
    collide2d(v, 1);
    border2d_impulse(v);
  }
}

CT_EXPORT void ct_verlet_set2f(CT_Verlet *v, size_t i, const float *pos) {
  size_t j  = i << 1;
  v->pos[j] = v->prev[j] = pos[0];
  v->pos[j + 1] = v->prev[j + 1] = pos[1];
  ct_spgrid_insert(&v->accel, pos, (void *)i);
}
