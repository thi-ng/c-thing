#include "math/verlet.h"
#include "common/dbg.h"

#ifdef CT_FEATURE_SSE

static void clear_forces2d(CT_Verlet2f *verlet) {
  float *f        = verlet->force;
  const __m128 gx = _mm_set1_ps(verlet->gravity[0]);
  const __m128 gy = _mm_set1_ps(verlet->gravity[1]);
  for (size_t num = verlet->num, i = 0, j = num; i < num; i += 4, j += 4) {
    *((__m128 *)&f[i]) = gx;
    *((__m128 *)&f[j]) = gy;
  }
}

static void apply_friction2d(CT_Verlet2f *verlet) {
  float *pos        = verlet->pos;
  const float *prev = verlet->prev;
  const __m128 f    = _mm_set1_ps(verlet->friction);
  for (size_t i = 0, num = verlet->num * 2; i < num; i += 4) {
    const __m128 p       = _mm_load_ps(&prev[i]);
    *((__m128 *)&pos[i]) = p + (_mm_load_ps(&pos[i]) - p) * f;
  }
}

static void update2d(CT_Verlet2f *verlet) {
  float *pos         = verlet->pos;
  float *prev        = verlet->prev;
  const float *force = verlet->force;
  const float *mass  = verlet->mass;
  const __m128 two   = _mm_set1_ps(2.0f);
  const __m128 dtsq  = _mm_set1_ps(verlet->timeStep * verlet->timeStep);
  for (size_t num = verlet->num, x = 0, y = num; x < num; x += 4, y += 4) {
    __m128 p              = _mm_load_ps(&pos[x]);
    __m128 q              = _mm_load_ps(&prev[x]);
    const __m128 m        = _mm_load_ps(&mass[x]);
    *((__m128 *)&prev[x]) = p;
    *((__m128 *)&pos[x])  = p * two - q + _mm_load_ps(&force[x]) * dtsq * m;
    p                     = _mm_load_ps(&pos[y]);
    q                     = _mm_load_ps(&prev[y]);
    *((__m128 *)&prev[y]) = p;
    *((__m128 *)&pos[y])  = p * two - q + _mm_load_ps(&force[y]) * dtsq * m;
  }
}

#else

static void clear_forces2d(CT_Verlet2f *verlet) {
  float *f       = verlet->force;
  const float gx = verlet->gravity[0];
  const float gy = verlet->gravity[1];
  for (size_t num = verlet->num, i = 0, j = num; i < num; i++, j++) {
    f[i] = gx;
    f[j] = gy;
  }
}

static void apply_friction2d(CT_Verlet2f *verlet) {
  float *p          = verlet->pos;
  const float *prev = verlet->prev;
  const float fr    = verlet->friction;
  for (size_t i = 0, num = verlet->num * 2; i < num; i++) {
    p[i] = prev[i] + (p[i] - prev[i]) * fr;
  }
}

static void update2d(CT_Verlet2f *verlet) {
  const float dt = verlet->timeStep * verlet->timeStep;
  float *pos     = verlet->pos;
  float *prev    = verlet->prev;
  float *f       = verlet->force;
  float *m       = verlet->mass;
  for (size_t num = verlet->num, x = 0, y = num; x < num; x++, y++) {
    float p = pos[x] * 2 - prev[x] + f[x] * dt * m[x];
    prev[x] = pos[x];
    pos[x]  = p;
    p       = pos[y] * 2 - prev[y] + f[y] * dt * m[x];
    prev[y] = pos[y];
    pos[y]  = p;
  }
}
#endif  // CT_FEATURE_SSE

static void separate2d(CT_Verlet2f *verlet) {
  CT_Vec2f p, q, sep;
  float *pos       = verlet->pos;
  float *force     = verlet->force;
  const float minD = verlet->minD * verlet->minD;
  for (size_t num = verlet->num, i = 0, j = num; i < num; i++) {
    ct_set2fxy(&p, pos[i], pos[i + num]);
    for (size_t j = i + 1; j < num; j++) {
      ct_set2fxy(&q, pos[j], pos[j + num]);
      ct_sub2fv(&q, &p, &sep);
      float d = ct_magsq2f(&sep);
      if (d < minD) {
        d = d > 0 ? sqrtf(d) : EPS;
        ct_mul2fn_imm(&sep, 0.5 * (verlet->minD - d) / d);
        //CT_INFO("%zu -> %zu, d=%f, sep=%f,%f", i, j, d, sep.x, sep.y);
        force[i] -= sep.x;
        force[j] += sep.x;
        force[i + num] -= sep.y;
        force[j + num] += sep.y;
      }
    }
  }
}

static void constrain2d(CT_Verlet2f *verlet) {
  float *pos      = verlet->pos;
  float *f        = verlet->force;
  const float bx1 = verlet->bounds[0];
  const float by1 = verlet->bounds[1];
  const float bx2 = verlet->bounds[2];
  const float by2 = verlet->bounds[3];
  for (size_t num = verlet->num, i = 0, j = num; i < num; i++, j++) {
    if (pos[i] < bx1) {
      f[i] += bx1 - pos[i];
    } else if (pos[i] > bx2) {
      f[i] += bx2 - pos[i];
    }
    if (pos[j] < by1) {
      f[j] += by1 - pos[j];
    } else if (pos[j] > by2) {
      f[j] += by2 - pos[j];
    }
  }
}

CT_EXPORT int ct_verlet_init(CT_Verlet2f *verlet, size_t num) {
  CT_CHECK(0 == (num & 3), "num must be multiple of 4");
  float *buf = malloc(7 * num * sizeof(float));
  CT_CHECK_MEM(buf);
  verlet->pos   = buf;
  verlet->prev  = buf + num * 2;
  verlet->force = buf + num * 4;
  verlet->mass  = buf + num * 6;
  verlet->num   = num;
  return 0;
fail:
  return 1;
}

CT_EXPORT void ct_verlet_trace(CT_Verlet2f *v) {
  CT_INFO(
      "pos: %p, prev: %p, force: %p,mass: %p, num: %zu, dt: %1.3f, friction: "
      "%1.3f, mind: %1.3f",
      v->pos, v->prev, v->force, v->mass, v->num, v->timeStep, v->friction,
      v->minD);
}

CT_EXPORT void ct_verlet_update2d(CT_Verlet2f *verlet) {
  clear_forces2d(verlet);
  separate2d(verlet);
  constrain2d(verlet);
  apply_friction2d(verlet);
  update2d(verlet);
}
