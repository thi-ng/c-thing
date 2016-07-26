#pragma once

#include "cthing.h"

#include "math/vec.h"

typedef struct {
  void **comps;
  size_t num;
  size_t width;
} CT_SOA;

#ifdef CT_FEATURE_SSE

#ifdef CT_FEATURE_AVX
#include <immintrin.h>
#define CT_SOA_WORD_SHIFT 3
#define CT_SOA_SET1 _mm256_set1_ps
#define CT_SOA_SQRT _mm256_sqrt_ps
#define CT_SOA_RSQRT _mm256_rsqrt_ps
#define CT_SOA_MIN _mm256_min_ps
#define CT_SOA_MAX _mm256_max_ps
typedef __m256 ct_soa_vec;
#else
#include <xmmintrin.h>
#define CT_SOA_WORD_SHIFT 2
#define CT_SOA_SET1 _mm_set1_ps
#define CT_SOA_SQRT _mm_sqrt_ps
#define CT_SOA_RSQRT _mm_rsqrt_ps
#define CT_SOA_MIN _mm_min_ps
#define CT_SOA_MAX _mm_max_ps
typedef __m128 ct_soa_vec;
#endif

#else  // NO SSE2

#define CT_SOA_WORD_SHIFT 0
#define CT_SOA_SET1
#define CT_SOA_SQRT sqrtf
#define CT_SOA_RSQRT 1.0f / sqrtf
#define CT_SOA_MIN CT_MIN
#define CT_SOA_MAX CT_MAX
typedef float ct_soa_vec;

#endif

#define CT_SOA_WORD_SIZE (1 << CT_SOA_WORD_SHIFT)
#define CT_SOA_SIZE_MASK (CT_SOA_WORD_SIZE - 1)

#define CT_SOA_PROLOGUE2_SCALAR_IMM(name)           \
  CT_SOA *ct_soa_##name##_imm(CT_SOA *a, float b) { \
    size_t n      = a->num >> CT_SOA_WORD_SHIFT;    \
    ct_soa_vec bb = CT_SOA_SET1(b);                 \
    for (size_t i = 0, w = a->width; i < w; i++) {  \
      ct_soa_vec *aa = (ct_soa_vec *)a->comps[i];   \
      for (size_t j = 0; j < n; j++)

#define CT_SOA_PROLOGUE2_FPTR_IMM(name)                    \
  CT_SOA *ct_soa_##name##_imm(CT_SOA *a, const float *b) { \
    size_t n = a->num >> CT_SOA_WORD_SHIFT;                \
    for (size_t i = 0, w = a->width; i < w; i++) {         \
      ct_soa_vec *aa = (ct_soa_vec *)a->comps[i];          \
      ct_soa_vec bb  = CT_SOA_SET1(b[i]);                  \
      for (size_t j = 0; j < n; j++)

#define CT_SOA_PROLOGUE2_IMM(name)                                             \
  CT_SOA *ct_soa_##name##_imm(CT_SOA *a, const CT_SOA *b) {                    \
    CT_CHECK(a->width == b->width && a->num == b->num, "a & b dims not same"); \
    size_t n = a->num >> CT_SOA_WORD_SHIFT;                                    \
    for (size_t i = 0, w = a->width; i < w; i++) {                             \
      ct_soa_vec *aa = (ct_soa_vec *)a->comps[i];                              \
      ct_soa_vec *bb = (ct_soa_vec *)b->comps[i];                              \
      for (size_t j = 0; j < n; j++)

#define CT_SOA_PROLOGUE3(name)                                           \
  CT_SOA *ct_soa_##name(const CT_SOA *a, const CT_SOA *b, CT_SOA *out) { \
    CT_CHECK(a->width == b->width && a->width == out->width &&           \
                 a->num == b->num && a->num == out->num,                 \
             "dims not same");                                           \
    size_t n = a->num >> CT_SOA_WORD_SHIFT;                              \
    for (size_t i = 0, w = a->width; i < w; i++) {                       \
      ct_soa_vec *aa = (ct_soa_vec *)a->comps[i];                        \
      ct_soa_vec *bb = (ct_soa_vec *)b->comps[i];                        \
      ct_soa_vec *oo = (ct_soa_vec *)out->comps[i];                      \
      for (size_t j = 0; j < n; j++)

#define CT_SOA2_PROLOGUE3(name)                                        \
  float *ct_soa_##name(const CT_SOA *a, const CT_SOA *b, float *out) { \
    CT_CHECK(a->width == 2 && b->width == 2 && a->num == b->num,       \
             "a & b dims not same");                                   \
    ct_soa_vec *ax = (ct_soa_vec *)a->comps[0];                        \
    ct_soa_vec *ay = (ct_soa_vec *)a->comps[1];                        \
    ct_soa_vec *bx = (ct_soa_vec *)b->comps[0];                        \
    ct_soa_vec *by = (ct_soa_vec *)b->comps[1];                        \
    ct_soa_vec *oo = (ct_soa_vec *)out;                                \
    for (size_t i = 0, n = a->num >> CT_SOA_WORD_SHIFT; i < n; i++)

#define CT_SOA3_PROLOGUE3(name)                                        \
  float *ct_soa_##name(const CT_SOA *a, const CT_SOA *b, float *out) { \
    CT_CHECK(a->width == 3 && b->width == 3 && a->num == b->num,       \
             "a & b dims not same");                                   \
    ct_soa_vec *ax = (ct_soa_vec *)a->comps[0];                        \
    ct_soa_vec *ay = (ct_soa_vec *)a->comps[1];                        \
    ct_soa_vec *az = (ct_soa_vec *)b->comps[2];                        \
    ct_soa_vec *bx = (ct_soa_vec *)b->comps[0];                        \
    ct_soa_vec *by = (ct_soa_vec *)b->comps[1];                        \
    ct_soa_vec *bz = (ct_soa_vec *)b->comps[2];                        \
    ct_soa_vec *oo = (ct_soa_vec *)out;                                \
    for (size_t i = 0, n = a->num >> CT_SOA_WORD_SHIFT; i < n; i++)

#define CT_SOA_EPILOGUE_IMM \
  }                         \
  return a;                 \
  fail:                     \
  return NULL;              \
  }

#define CT_SOA_EPILOGUE3 \
  }                      \
  return out;            \
  fail:                  \
  return NULL;           \
  }

#define CT_SOA_EPILOGUE3_ALT \
  return out;                \
  fail:                      \
  return NULL;               \
  }

CT_SOA *ct_soa_new(size_t width, size_t num, size_t stride);
int ct_soa_init(CT_SOA *a, void **comps, size_t width, size_t num);
void ct_soa_free(CT_SOA *s);

CT_SOA *ct_soa_add1f_imm(CT_SOA *a, float b);
CT_SOA *ct_soa_sub1f_imm(CT_SOA *a, float b);
CT_SOA *ct_soa_mul1f_imm(CT_SOA *a, float b);
CT_SOA *ct_soa_div1f_imm(CT_SOA *a, float b);

CT_SOA *ct_soa_add1fp_imm(CT_SOA *a, const float *b);
CT_SOA *ct_soa_sub1fp_imm(CT_SOA *a, const float *b);
CT_SOA *ct_soa_mul1fp_imm(CT_SOA *a, const float *b);
CT_SOA *ct_soa_div1fp_imm(CT_SOA *a, const float *b);

CT_SOA *ct_soa_add_imm(CT_SOA *a, const CT_SOA *b);
CT_SOA *ct_soa_sub_imm(CT_SOA *a, const CT_SOA *b);
CT_SOA *ct_soa_mul_imm(CT_SOA *a, const CT_SOA *b);
CT_SOA *ct_soa_div_imm(CT_SOA *a, const CT_SOA *b);

CT_SOA *ct_soa_min_imm(CT_SOA *a, const CT_SOA *b);
CT_SOA *ct_soa_max_imm(CT_SOA *a, const CT_SOA *b);

CT_SOA *ct_soa_add(const CT_SOA *a, const CT_SOA *b, CT_SOA *out);
CT_SOA *ct_soa_sub(const CT_SOA *a, const CT_SOA *b, CT_SOA *out);
CT_SOA *ct_soa_mul(const CT_SOA *a, const CT_SOA *b, CT_SOA *out);
CT_SOA *ct_soa_div(const CT_SOA *a, const CT_SOA *b, CT_SOA *out);

float *ct_soa_dot2(const CT_SOA *a, const CT_SOA *b, float *out);
float *ct_soa_dist2(const CT_SOA *a, const CT_SOA *b, float *out);

CT_SOA *ct_soa_normalize2f_imm(CT_SOA *a, float len);
CT_SOA *ct_soa_normalize3f_imm(CT_SOA *a, float len);

ct_inline CT_Vec2f *ct_soa_get2f(const CT_SOA *s, size_t idx, CT_Vec2f *v) {
  float **c = (float **)s->comps;
  v->x      = c[0][idx];
  v->y      = c[1][idx];
  return v;
}

ct_inline CT_Vec3f *ct_soa_get3f(const CT_SOA *s, size_t idx, CT_Vec3f *v) {
  float **c = (float **)s->comps;
  v->x      = c[0][idx];
  v->y      = c[1][idx];
  v->z      = c[2][idx];
  return v;
}

ct_inline CT_Vec4f *ct_soa_get4f(const CT_SOA *s, size_t idx, CT_Vec4f *v) {
  float **c = (float **)s->comps;
  v->x      = c[0][idx];
  v->y      = c[1][idx];
  v->z      = c[2][idx];
  v->w      = c[3][idx];
  return v;
}

ct_inline CT_SOA *ct_soa_set2f(CT_SOA *s, size_t idx, const CT_Vec2f *v) {
  float **c = (float **)s->comps;
  c[0][idx] = v->x;
  c[1][idx] = v->y;
  return s;
}

ct_inline CT_SOA *ct_soa_set3f(CT_SOA *s, size_t idx, const CT_Vec3f *v) {
  float **c = (float **)s->comps;
  c[0][idx] = v->x;
  c[1][idx] = v->y;
  c[2][idx] = v->z;
  return s;
}

ct_inline CT_SOA *ct_soa_set4f(CT_SOA *s, size_t idx, const CT_Vec4f *v) {
  float **c = (float **)s->comps;
  c[0][idx] = v->x;
  c[1][idx] = v->y;
  c[2][idx] = v->z;
  c[3][idx] = v->w;
  return s;
}
