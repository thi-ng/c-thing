#include <string.h>

#include "data/array.h"
#include "math/math.h"

float *ct_array_add2_f32(const float *a,
                         const float *b,
                         float *out,
                         size_t num,
                         size_t fstridea,
                         size_t fstrideb,
                         size_t fstrideo) {
  float *ptr = out, *aa = (float *)a, *bb = (float *)b;
  while (num--) {
    *ptr = *aa + *bb;
    aa += fstridea;
    bb += fstrideb;
    ptr += fstrideo;
  }
  return out;
}

float *ct_array_mul2_f32(const float *a,
                         const float *b,
                         float *out,
                         size_t num,
                         size_t fstridea,
                         size_t fstrideb,
                         size_t fstrideo) {
  float *ptr = out, *aa = (float *)a, *bb = (float *)b;
  while (num--) {
    *ptr = *aa * *bb;
    aa += fstridea;
    bb += fstrideb;
    ptr += fstrideo;
  }
  return out;
}

float ct_array_reduce_sum_f32(const float *p, size_t num, size_t fstride) {
  float sum  = 0;
  float *ptr = (float *)p;
  while (num--) {
    sum += *ptr;
    ptr += fstride;
  }
  return sum;
}

intptr_t ct_array_reduce_sum_i32(const int32_t *p, size_t num, size_t istride) {
  intptr_t sum = 0;
  int32_t *ptr = (int32_t *)p;
  while (num--) {
    sum += *ptr;
    ptr += istride;
  }
  return sum;
}

uintptr_t ct_array_reduce_sum_u32(const uint32_t *p,
                                  size_t num,
                                  size_t istride) {
  uintptr_t sum = 0;
  uint32_t *ptr = (uint32_t *)p;
  while (num--) {
    sum += *ptr;
    ptr += istride;
  }
  return sum;
}

float *ct_array_reverse_f32_imm(float *ptr, size_t num, size_t fstride) {
  CT_CHECK(fstride <= 32, "fstride > 32");
  uint8_t tmp[128];
  switch (fstride) {
    case 1: {
      for (size_t i = 0, n2 = num >> 1, end = (num - 1); i < n2; i++, end--) {
        float tmp = ptr[i];
        ptr[i]    = ptr[end];
        ptr[end]  = tmp;
      }
    } break;
    case 2: {
      for (size_t i = 0, n2 = ((num >> 1) << 1), end = (num - 1) << 1; i < n2;
           i += 2, end -= 2) {
        float tmp    = ptr[i];
        float tmp2   = ptr[i + 1];
        ptr[i]       = ptr[end];
        ptr[i + 1]   = ptr[end + 1];
        ptr[end]     = tmp;
        ptr[end + 1] = tmp2;
      }
    } break;
    default: {
      size_t stride = fstride * sizeof(float);
      for (size_t i = 0, n2 = num / 2 * fstride, end = (num - 1) * fstride;
           i < n2; i += fstride, end -= fstride) {
        memcpy(tmp, &ptr[i], stride);
        memcpy(&ptr[i], &ptr[end], stride);
        memcpy(&ptr[end], tmp, stride);
      }
    }
  }
  return ptr;
fail:
  return NULL;
}

void *ct_array_reverse_imm(void *ptr, size_t num, size_t stride) {
  CT_CHECK(stride <= 128, "stride > 128");
  uint8_t tmp[128];
  uint8_t *p = (uint8_t *)ptr;
  for (size_t i = 0, n2 = num / 2 * stride, end = (num - 1) * stride; i < n2;
       i += stride, end -= stride) {
    memcpy(tmp, &p[i], stride);
    memcpy(&p[i], &p[end], stride);
    memcpy(&p[end], tmp, stride);
  }
  return ptr;
fail:
  return NULL;
}

// TODO verify why slower...
/*
#ifdef CT_FEATURE_SSE
#include <xmmintrin.h>

void *ct_array_reverse8_imm(void *ptr, size_t num) {
  __m64 *iptr = (__m64*)ptr;
  for (size_t i = 0, n2 = num >> 1, end = num - 1; i < n2; i++, end--) {
    //CT_INFO("swap: %p, %p", &iptr[i], &iptr[end]);
    SWAP(__m64, iptr[i], iptr[end]);
  }
  return ptr;
}

#else */

void *ct_array_reverse8_imm(void *ptr, size_t num) {
  uint32_t *iptr = (uint32_t *)ptr;
  for (size_t i = 0, end = num * 2 - 2; i < num; i += 2, end -= 2) {
    SWAP(uint32_t, iptr[i], iptr[end]);
    SWAP(uint32_t, iptr[i + 1], iptr[end + 1]);
  }
  return ptr;
}

// #endif

void *ct_array_reverse(const void *src, size_t num, size_t stride, void *dest) {
  CT_CHECK(stride <= 128, "stride > 128");
  uint8_t *s = (uint8_t *)src;
  uint8_t *d = (uint8_t *)dest;
  for (size_t i = 0, len = num * stride, end = len - stride; i < len;
       i += stride, end -= stride) {
    memcpy(&d[end], &s[i], stride);
  }
  return dest;
fail:
  return NULL;
}

int ct_array_deltaeq_f32(const float *a,
                         const float *b,
                         float eps,
                         size_t num) {
  for (size_t i = 0; i < num; i++) {
    if (!ct_deltaeqf(a[i], b[i], eps)) {
      return 0;
    }
  }
  return 0;
}

int ct_array_compare_f32(const float *a,
                         const float *b,
                         float eps,
                         size_t num) {
  for (size_t i = 0; i < num; i++) {
    float delta = a[i] - b[i];
    if (delta < -eps) {
      return -1;
    } else if (delta > eps) {
      return 1;
    }
  }
  return 0;
}

int ct_array_tostring_f32(char *buf,
                          int bufsz,
                          const float *p,
                          size_t num,
                          size_t fstride) {
  if (snprintf(buf, bufsz, "[") > 0) {
    size_t j = 1;
    bufsz -= j;
    float *ptr = (float *)p;
    for (size_t i = 0; i < num && bufsz > 0; i++) {
      int res = snprintf(&buf[j], bufsz, "%1.4f", *ptr);
      if (res < 0) {
        return -1;
      }
      if (i < num - 1) {
        buf[j + res] = ' ';
        res++;
      }
      bufsz -= res;
      j += res;
      ptr += fstride;
    }
    if (bufsz > 1) {
      buf[j]     = ']';
      buf[j + 1] = 0;
      return j + 1;
    }
  }
  return -1;
}
