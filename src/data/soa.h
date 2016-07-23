#pragma once

#include "cthing.h"

#include "math/vec.h"

typedef struct {
  float *comps[4];
  size_t num;
  uint8_t width;
} CT_SOA;

#ifdef CT_FEATURE_SSE

#include <xmmintrin.h>

#define CT_SOA_PROLOGUE1(name)                             \
  CT_SOA *ct_soa_##name##_imm(CT_SOA *a, const float *b) { \
    CT_CHECK(a->num == b->num, "a & b dim not same");      \
    size_t n = a->num >> 2;                                \
    for (size_t i = 0, w = a->width; i < w; i++) {         \
      __m128 *aa = (__m128 *)a->comps[i];                  \
      __m128 bb  = _mm_set1_ps(b[i]);                      \
      for (size_t j = 0; j < n; j++)

#define CT_SOA_PROLOGUE2(name)                                                 \
  CT_SOA *ct_soa_##name##_imm(CT_SOA *a, const CT_SOA *b) {                    \
    CT_CHECK(a->width == b->width && a->num == b->num, "a & b dims not same"); \
    size_t n = a->num >> 2;                                                    \
    for (size_t i = 0, w = a->width; i < w; i++) {                             \
      __m128 *aa = (__m128 *)a->comps[i];                                      \
      __m128 *bb = (__m128 *)b->comps[i];                                      \
      for (size_t j = 0; j < n; j++)

#define CT_SOA_EPILOGUE \
  }                     \
  return a;             \
  fail:                 \
  return NULL;          \
  }

#else

#endif

int ct_soa_init(CT_SOA *a, float *comps, size_t width, size_t num);

CT_SOA *ct_soa_add1_imm(CT_SOA *a, const float *b);
CT_SOA *ct_soa_sub1_imm(CT_SOA *a, const float *b);
CT_SOA *ct_soa_mul1_imm(CT_SOA *a, const float *b);
CT_SOA *ct_soa_div1_imm(CT_SOA *a, const float *b);

CT_SOA *ct_soa_add_imm(CT_SOA *a, const CT_SOA *b);
CT_SOA *ct_soa_sub_imm(CT_SOA *a, const CT_SOA *b);
CT_SOA *ct_soa_mul_imm(CT_SOA *a, const CT_SOA *b);
CT_SOA *ct_soa_div_imm(CT_SOA *a, const CT_SOA *b);

float *ct_soa_dot2(const CT_SOA *a, const CT_SOA *b, float *out);
float *ct_soa_dist2(const CT_SOA *a, const CT_SOA *b, float *out);

ct_inline CT_Vec2f *ct_soa_get2f(const CT_SOA *s, size_t idx, CT_Vec2f *v) {
  v->x = s->comps[0][idx];
  v->y = s->comps[1][idx];
  return v;
}

ct_inline CT_Vec3f *ct_soa_get3f(const CT_SOA *s, size_t idx, CT_Vec3f *v) {
  v->x = s->comps[0][idx];
  v->y = s->comps[1][idx];
  v->z = s->comps[2][idx];
  return v;
}

ct_inline CT_Vec4f *ct_soa_get4f(const CT_SOA *s, size_t idx, CT_Vec4f *v) {
  v->x = s->comps[0][idx];
  v->y = s->comps[1][idx];
  v->z = s->comps[2][idx];
  v->w = s->comps[3][idx];
  return v;
}

ct_inline CT_SOA *ct_soa_set2f(CT_SOA *s, size_t idx, const CT_Vec2f *v) {
  s->comps[0][idx] = v->x;
  s->comps[1][idx] = v->y;
  return s;
}

ct_inline CT_SOA *ct_soa_set3f(CT_SOA *s, size_t idx, const CT_Vec3f *v) {
  s->comps[0][idx] = v->x;
  s->comps[1][idx] = v->y;
  s->comps[2][idx] = v->z;
  return s;
}

ct_inline CT_SOA *ct_soa_set4f(CT_SOA *s, size_t idx, const CT_Vec4f *v) {
  s->comps[0][idx] = v->x;
  s->comps[1][idx] = v->y;
  s->comps[2][idx] = v->z;
  s->comps[3][idx] = v->w;
  return s;
}
