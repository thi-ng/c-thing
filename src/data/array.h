#include "cthing.h"

float *ct_array_add2_f32(const float *a,
                         const float *b,
                         float *out,
                         size_t num,
                         size_t fstridea,
                         size_t fstrideb,
                         size_t fstrideo);
float *ct_array_mul2_f32(const float *a,
                         const float *b,
                         float *out,
                         size_t num,
                         size_t fstridea,
                         size_t fstrideb,
                         size_t fstrideo);

float ct_array_reduce_sum_f32(const float *p, size_t num, size_t fstride);
intptr_t ct_array_reduce_sum_i32(const int32_t *p, size_t num, size_t istride);
uintptr_t ct_array_reduce_sum_u32(const uint32_t *p,
                                  size_t num,
                                  size_t istride);

float *ct_array_reverse_f32_imm(float *ptr, size_t num, size_t fstride);
void *ct_array_reverse_imm(void *ptr, size_t num, size_t stride);
void *ct_array_reverse8_imm(void *ptr, size_t num);
void *ct_array_reverse(const void *src, size_t num, size_t stride, void *out);

int ct_array_deltaeq_f32(const float *a, const float *b, float eps, size_t num);
int ct_array_compare_f32(const float *a, const float *b, float eps, size_t num);
int ct_array_tostring_f32(char *buf,
                          int bufsz,
                          const float *p,
                          size_t num,
                          size_t fstride);
