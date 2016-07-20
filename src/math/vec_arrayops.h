#pragma once

#include "math/vec.h"

#define VEC_ARRAYOP(name, type, ptype, fn)                                   \
  ct_export void ct_##name(ptype* ptr, type* t, size_t num, size_t stride) { \
    if (num > 0) {                                                           \
      while (num--) {                                                        \
        fn((type*)ptr, t);                                                   \
        ptr += stride;                                                       \
      }                                                                      \
    }                                                                        \
  }

CT_BEGIN_DECLS

int ct_deltaeqfp(const float* a, const float* b, float eps, size_t num);
int ct_comparefp(const float* a, const float* b, float eps, size_t num);
int ct_tostringfp(char* buf, int bufsz, const float* p, size_t num);

void ct_translate2f(float* ptr, CT_Vec2f* t, size_t num, size_t fstride);
void ct_scale2f(float* ptr, CT_Vec2f* t, size_t num, size_t fstride);
void ct_translate3f(float* ptr, CT_Vec3f* t, size_t num, size_t fstride);
void ct_scale3f(float* ptr, CT_Vec3f* t, size_t num, size_t fstride);
void ct_translate4f(float* ptr, CT_Vec4f* t, size_t num, size_t fstride);
void ct_scale4f(float* ptr, CT_Vec4f* t, size_t num, size_t fstride);

CT_Vec2f* ct_closest_point2f(float* ptr, CT_Vec2f* p, size_t num,
                             size_t fstride);

CT_Vec3f* ct_closest_point3f(float* ptr, CT_Vec3f* p, size_t num,
                             size_t fstride);

int ct_bounds2fp(float* ptr, size_t num, size_t fstride, CT_Vec2f* min,
                 CT_Vec2f* max);
int ct_bounds3fp(float* ptr, size_t num, size_t fstride, CT_Vec3f* min,
                 CT_Vec3f* max);

size_t ct_convexhull2f(CT_Vec2f* ptr, size_t num, CT_Vec2f* hull);

CT_END_DECLS
