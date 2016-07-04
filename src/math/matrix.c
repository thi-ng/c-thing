#include <math.h>
#include <string.h>

#include "math/matrix.h"

#ifdef CT_FEATURE_SSE

CT_Mat4f *ct_mat4f_set_identity(CT_Mat4f *m) {
  return NULL;
}

CT_Mat4f *ct_mat4f_mul_imm(CT_Mat4f *a, const CT_Mat4f *b) {
  return NULL;
}

CT_Mat4f *ct_mat4f_scalen_imm(CT_Mat4f *m, float n) {
  return NULL;
}

CT_Mat4f *ct_mat4f_translate3fp_imm(CT_Mat4f *m, const float *t) {
  return NULL;
}

CT_Mat4f *ct_mat4f_rotatex_imm(CT_Mat4f *m, float theta) {
  return NULL;
}

CT_Mat4f *ct_mat4f_rotatey_imm(CT_Mat4f *m, float theta) {
  return NULL;
}

CT_Mat4f *ct_mat4f_rotatez_imm(CT_Mat4f *m, float theta) {
  return NULL;
}

#else

CT_Mat4f *ct_mat4f_set_identity(CT_Mat4f *m) {
  memset(m, 0, 16 * sizeof(float));
  m->m00 = m->m11 = m->m22 = m->m33 = 1.0f;
  return m;
}

CT_Mat4f *ct_mat4f_mul_imm(CT_Mat4f *a, const CT_Mat4f *b) {
  CT_Mat4f t = *a;
  float i = b->m00, j = b->m01, k = b->m02, l = b->m03;
  a->mat[0] = t.m00 * i + t.m10 * j + t.m20 * k + t.m30 * l;
  a->mat[1] = t.m01 * i + t.m11 * j + t.m21 * k + t.m31 * l;
  a->mat[2] = t.m02 * i + t.m12 * j + t.m22 * k + t.m32 * l;
  a->mat[3] = t.m03 * i + t.m13 * j + t.m23 * k + t.m33 * l;

  i = b->m10, j = b->m11, k = b->m12, l = b->m13;
  a->mat[4] = t.m00 * i + t.m10 * j + t.m20 * k + t.m30 * l;
  a->mat[5] = t.m01 * i + t.m11 * j + t.m21 * k + t.m31 * l;
  a->mat[6] = t.m02 * i + t.m12 * j + t.m22 * k + t.m32 * l;
  a->mat[7] = t.m03 * i + t.m13 * j + t.m23 * k + t.m33 * l;

  i = b->m20, j = b->m21, k = b->m22, l = b->m23;
  a->mat[8]  = t.m00 * i + t.m10 * j + t.m20 * k + t.m30 * l;
  a->mat[9]  = t.m01 * i + t.m11 * j + t.m21 * k + t.m31 * l;
  a->mat[10] = t.m02 * i + t.m12 * j + t.m22 * k + t.m32 * l;
  a->mat[11] = t.m03 * i + t.m13 * j + t.m23 * k + t.m33 * l;

  i = b->m30, j = b->m31, k = b->m32, l = b->m33;
  a->mat[12] = t.m00 * i + t.m10 * j + t.m20 * k + t.m30 * l;
  a->mat[13] = t.m01 * i + t.m11 * j + t.m21 * k + t.m31 * l;
  a->mat[14] = t.m02 * i + t.m12 * j + t.m22 * k + t.m32 * l;
  a->mat[15] = t.m03 * i + t.m13 * j + t.m23 * k + t.m33 * l;
  return a;
}

CT_Mat4f *ct_mat4f_scalen_imm(CT_Mat4f *m, float n) {
  m->m00 *= n;
  m->m01 *= n;
  m->m02 *= n;
  m->m03 *= n;
  m->m10 *= n;
  m->m11 *= n;
  m->m12 *= n;
  m->m13 *= n;
  m->m20 *= n;
  m->m21 *= n;
  m->m22 *= n;
  m->m23 *= n;
  return m;
}

CT_Mat4f *ct_mat4f_translate3fp_imm(CT_Mat4f *m, const float *t) {
  float x = t[0], y = t[1], z = t[2];
  m->m30 += m->m00 * x + m->m10 * y + m->m20 * z;
  m->m31 += m->m01 * x + m->m11 * y + m->m21 * z;
  m->m32 += m->m02 * x + m->m12 * y + m->m22 * z;
  m->m33 += m->m03 * x + m->m13 * y + m->m23 * z;
  return m;
}

CT_Mat4f *ct_mat4f_rotatex_imm(CT_Mat4f *m, float theta) {
  float s = sinf(theta);
  float c = cosf(theta);
  float i = m->m10, j = m->m11, k = m->m12, l = m->m13;
  m->m10 = i * c + m->m20 * s;
  m->m11 = j * c + m->m21 * s;
  m->m12 = k * c + m->m22 * s;
  m->m13 = l * c + m->m23 * s;
  m->m20 = m->m20 * c - i * s;
  m->m21 = m->m21 * c - j * s;
  m->m22 = m->m22 * c - k * s;
  m->m23 = m->m23 * c - l * s;
  return m;
}

CT_Mat4f *ct_mat4f_rotatey_imm(CT_Mat4f *m, float theta) {
  float s = sinf(theta);
  float c = cosf(theta);
  float i = m->m00, j = m->m01, k = m->m02, l = m->m03;
  m->m00 = i * c - m->m20 * s;
  m->m01 = j * c - m->m21 * s;
  m->m02 = k * c - m->m22 * s;
  m->m03 = l * c - m->m23 * s;
  m->m20 = i * s + m->m20 * c;
  m->m21 = j * s + m->m21 * c;
  m->m22 = k * s + m->m22 * c;
  m->m23 = l * s + m->m23 * c;
  return m;
}

CTCT_Mat4f *ct_mat4f_rotatez_imm(CT_Mat4f *m, float theta) {
  float s = sinf(theta);
  float c = cosf(theta);
  float i = m->m00, j = m->m01, k = m->m02, l = m->m03;
  m->m00 = i * c + m->m10 * s;
  m->m01 = j * c + m->m11 * s;
  m->m02 = k * c + m->m12 * s;
  m->m03 = l * c + m->m13 * s;
  m->m10 = m->m10 * c - i * s;
  m->m11 = m->m11 * c - j * s;
  m->m12 = m->m12 * c - k * s;
  m->m13 = m->m13 * c - l * s;
  return m;
}

#endif
