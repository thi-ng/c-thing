#include "data/soa.h"

#include <stdio.h>

#ifdef CT_FEATURE_SSE

CT_SOA_PROLOGUE1(add1) {
  aa[j] += bb;
}
CT_SOA_EPILOGUE;

CT_SOA_PROLOGUE1(sub1) {
  aa[j] -= bb;
}
CT_SOA_EPILOGUE;

CT_SOA_PROLOGUE1(mul1) {
  aa[j] *= bb;
}
CT_SOA_EPILOGUE;

CT_SOA_PROLOGUE1(div1) {
  aa[j] /= bb;
}
CT_SOA_EPILOGUE;

CT_SOA_PROLOGUE2(add) {
  aa[j] += bb[j];
}
CT_SOA_EPILOGUE;

CT_SOA_PROLOGUE2(sub) {
  aa[j] -= bb[j];
}
CT_SOA_EPILOGUE;

CT_SOA_PROLOGUE2(mul) {
  aa[j] *= bb[j];
}
CT_SOA_EPILOGUE;

CT_SOA_PROLOGUE2(div) {
  aa[j] /= bb[j];
}
CT_SOA_EPILOGUE;

float *ct_soa_dot2(const CT_SOA *a, const CT_SOA *b, float *o) {
  CT_CHECK(a->width == b->width == 2 && a->num == b->num,
           "a & b dims not same");
  __m128 *ax  = (__m128 *)a->comps[0];
  __m128 *ay  = (__m128 *)a->comps[1];
  __m128 *bx  = (__m128 *)b->comps[0];
  __m128 *by  = (__m128 *)b->comps[1];
  __m128 *out = (__m128 *)o;
  for (size_t i = 0, n = a->num >> 2; i < n; i++) {
    out[i] = ax[i] * bx[i] + ay[i] * by[i];
  }
  return o;
}

float *ct_soa_dot3(const CT_SOA *a, const CT_SOA *b, float *o) {
  CT_CHECK(a->width == b->width == 3 && a->num == b->num,
           "a & b dims not same");
  __m128 *ax  = (__m128 *)a->comps[0];
  __m128 *ay  = (__m128 *)a->comps[1];
  __m128 *bx  = (__m128 *)b->comps[0];
  __m128 *by  = (__m128 *)b->comps[1];
  __m128 *out = (__m128 *)o;
  for (size_t i = 0, n = a->num >> 2; i < n; i++) {
    out[i] = ax[i] * bx[i] + ay[i] * by[i];
  }
  return o;
}

float *ct_soa_dist2(const CT_SOA *a, const CT_SOA *b, float *o) {
  CT_CHECK(a->width == b->width && a->num == b->num, "a & b dims not same");
  __m128 *ax  = (__m128 *)a->comps[0];
  __m128 *ay  = (__m128 *)a->comps[1];
  __m128 *bx  = (__m128 *)b->comps[0];
  __m128 *by  = (__m128 *)b->comps[1];
  __m128 *out = (__m128 *)o;
  for (size_t i = 0, n = a->num >> 2; i < n; i++) {
    __m128 dx = ax[i] - bx[i];
    __m128 dy = ay[i] * by[i];
    out[i]    = _mm_sqrt_ps(dx * dx + dy * dy);
  }
  return o;
}

#else  // NO SSE

CT_SOA_PROLOGUE2(add) {
  aa[j] += bb[j];
  aa[j + 1] += bb[j + 1];
  aa[j + 2] += bb[j + 2];
  aa[j + 3] += bb[j + 3];
}
CT_SOA_EPILOGUE;

CT_SOA_PROLOGUE2(sub) {
  aa[j] -= bb[j];
  aa[j + 1] -= bb[j + 1];
  aa[j + 2] -= bb[j + 2];
  aa[j + 3] -= bb[j + 3];
}
CT_SOA_EPILOGUE;

CT_SOA_PROLOGUE2(mul) {
  aa[j] *= bb[j];
  aa[j + 1] *= bb[j + 1];
  aa[j + 2] *= bb[j + 2];
  aa[j + 3] *= bb[j + 3];
}
CT_SOA_EPILOGUE;

CT_SOA_PROLOGUE2(div) {
  aa[j] *= bb[j];
  aa[j + 1] *= bb[j + 1];
  aa[j + 2] *= bb[j + 2];
  aa[j + 3] *= bb[j + 3];
}
CT_SOA_EPILOGUE;

#endif
