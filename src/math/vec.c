#include <float.h>
#include <stdio.h>

#include "math/vec.h"

ct_export CT_Vec2f *ct_vec2f(float x, float y, CT_MPool *mpool) {
  CT_Vec2f *v = CT_MP_ALLOC(mpool, CT_Vec2f);
  if (v != NULL) {
    v->x = x;
    v->y = y;
  }
  return v;
}

ct_export CT_Vec2f *ct_vec2fn(float n, CT_MPool *mpool) {
  return ct_vec2f(n, n, mpool);
}

ct_export CT_Vec3f *ct_vec3f(float x, float y, float z, CT_MPool *mpool) {
  CT_Vec3f *v = CT_MP_ALLOC(mpool, CT_Vec3f);
  if (v != NULL) {
#ifdef CT_FEATURE_SSE
    v->mmval = _mm_set_ps(0, z, y, x);
#else
    v->x = x;
    v->y = y;
    v->z = z;
#endif
  }
  return v;
}

ct_export CT_Vec3f *ct_vec3fn(float n, CT_MPool *mpool) {
  CT_Vec3f *v = CT_MP_ALLOC(mpool, CT_Vec3f);
  if (v != NULL) {
#ifdef CT_FEATURE_SSE
    v->mmval = _mm_set1_ps(n);
#else
    v->x = v->y = v->z = n;
#endif
  }
  return v;
}

ct_export CT_Vec4f *ct_vec4f(float x, float y, float z, float w,
                             CT_MPool *mpool) {
  CT_Vec4f *v = CT_MP_ALLOC(mpool, CT_Vec4f);
  if (v != NULL) {
#ifdef CT_FEATURE_SSE
    v->mmval = _mm_set_ps(w, z, y, x);
#else
    v->x               = x;
    v->y               = y;
    v->z               = z;
    v->w               = w;
#endif
  }
  return v;
}

ct_export CT_Vec4f *ct_vec4fn(float n, CT_MPool *mpool) {
  CT_Vec4f *v = CT_MP_ALLOC(mpool, CT_Vec4f);
  if (v != NULL) {
#ifdef CT_FEATURE_SSE
    v->mmval = _mm_set1_ps(n);
#else
    v->x = v->y = v->z = v->w = n;
#endif
  }
  return v;
}

// ---------- array ops

VEC_ARRAYOP(translate2f, CT_Vec2f, float, ct_add2fv_imm)
VEC_ARRAYOP(scale2f, CT_Vec2f, float, ct_mul2fv_imm)
VEC_ARRAYOP(translate3f, CT_Vec3f, float, ct_add3fv_imm)
VEC_ARRAYOP(scale3f, CT_Vec3f, float, ct_mul3fv_imm)
VEC_ARRAYOP(translate4f, CT_Vec4f, float, ct_add4fv_imm)
VEC_ARRAYOP(scale4f, CT_Vec4f, float, ct_mul4fv_imm)

ct_export CT_Vec2f *ct_closest_point2f(float *ptr, CT_Vec2f *p, size_t num,
                                       size_t fstride) {
  float minD        = FLT_MAX;
  CT_Vec2f *closest = NULL;
  if (num > 0) {
    while (num--) {
      float d = ct_distsq2fv((CT_Vec2f *)ptr, p);
      if (d < minD) {
        closest = (CT_Vec2f *)ptr;
        minD    = d;
      }
      ptr += fstride;
    }
  }
  return closest;
}

ct_export CT_Vec3f *ct_closest_point3f(float *ptr, CT_Vec3f *p, size_t num,
                                       size_t fstride) {
  float minD        = FLT_MAX;
  CT_Vec3f *closest = NULL;
  if (num > 0) {
    while (num--) {
      const float d = ct_distsq3fv((CT_Vec3f *)ptr, p);
      if (d < minD) {
        closest = (CT_Vec3f *)ptr;
        minD    = d;
      }
      ptr += fstride;
    }
  }
  return closest;
}

ct_export int ct_bounds2fp(float *ptr, size_t num, size_t fstride,
                           CT_Vec2f *min, CT_Vec2f *max) {
  if (num > 0) {
    ct_set2fn(min, FLT_MAX);
    ct_set2fn(max, -FLT_MAX);
    while (num--) {
      ct_min2fv_imm(min, (CT_Vec2f *)ptr);
      ct_max2fv_imm(max, (CT_Vec2f *)ptr);
      ptr += fstride;
    }
    return 0;
  }
  return 1;
}

ct_export int ct_bounds3fp(float *ptr, size_t num, size_t fstride,
                           CT_Vec3f *min, CT_Vec3f *max) {
  if (num > 0) {
    ct_set3fn(min, FLT_MAX);
    ct_set3fn(max, -FLT_MAX);
    while (num--) {
      ct_min3fv_imm(min, (CT_Vec3f *)ptr);
      ct_max3fv_imm(max, (CT_Vec3f *)ptr);
      ptr += fstride;
    }
    return 0;
  }
  return 1;
}

int ct_deltaeqfp(const float *a, const float *b, float eps, size_t num) {
  for (size_t i = 0; i < num; i++) {
    if (!ct_deltaeqf(a[i], b[i], eps)) return 0;
  }
  return 0;
}

int ct_comparefp(const float *a, const float *b, float eps, size_t num) {
  for (size_t i = 0; i < num; i++) {
    float delta = a[i] - b[i];
    if (delta < -eps)
      return -1;
    else if (delta > eps)
      return 1;
  }
  return 0;
}

int ct_tostringfp(char *buf, int bufsz, const float *p, size_t num) {
  if (snprintf(buf, bufsz, "[") > 0) {
    size_t j = 1;
    bufsz -= j;
    for (size_t i = 0; i < num && bufsz > 0; i++) {
      int res = snprintf(&buf[j], bufsz, "%1.4f", p[i]);
      if (res < 0) return -1;
      if (i < num - 1) {
        buf[j + res] = ' ';
        res++;
      }
      bufsz -= res;
      j += res;
    }
    if (bufsz > 1) {
      buf[j]     = ']';
      buf[j + 1] = 0;
      return j + 1;
    }
  }
  return -1;
}
