#include <math.h>
#include <string.h>

#include "math/matrix.h"

#ifdef CT_FEATURE_SSE

#include <xmmintrin.h>
#ifdef CT_FEATURE_SSE4
#include <smmintrin.h>
#else
#endif

ct_export CT_Mat4f *ct_mat4f_set_identity(CT_Mat4f *m) {
  _mm_store_ps(&m->m00, (__m128){1, 0, 0, 0});
  _mm_store_ps(&m->m10, (__m128){0, 1, 0, 0});
  _mm_store_ps(&m->m20, (__m128){0, 0, 1, 0});
  _mm_store_ps(&m->m30, (__m128){0, 0, 0, 1});
  return m;
}

ct_export CT_Mat4f *ct_mat4f_transpose_imm(CT_Mat4f *m) {
  __m128 a  = _mm_load_ps(&m->m00);
  __m128 b  = _mm_load_ps(&m->m10);
  __m128 c  = _mm_load_ps(&m->m20);
  __m128 d  = _mm_load_ps(&m->m30);
  __m128 ta = _mm_unpacklo_ps(a, b);
  __m128 tb = _mm_unpacklo_ps(c, d);
  _mm_store_ps(&m->m00, _mm_movelh_ps(ta, tb));
  _mm_store_ps(&m->m10, _mm_movehl_ps(tb, ta));
  ta = _mm_unpackhi_ps(a, b);
  tb = _mm_unpackhi_ps(c, d);
  _mm_store_ps(&m->m20, _mm_movelh_ps(ta, tb));
  _mm_store_ps(&m->m30, _mm_movehl_ps(tb, ta));
  return m;
}

ct_export CT_Mat4f *ct_mat4f_mul_imm(CT_Mat4f *a, const CT_Mat4f *b) {
  __m128 a0 = _mm_load_ps(&a->m00);
  __m128 a1 = _mm_load_ps(&a->m10);
  __m128 a2 = _mm_load_ps(&a->m20);
  __m128 a3 = _mm_load_ps(&a->m30);
  _mm_store_ps(&a->m00, a0 * _mm_set1_ps(b->m00) + a1 * _mm_set1_ps(b->m01) +
                            a2 * _mm_set1_ps(b->m02) +
                            a3 * _mm_set1_ps(b->m03));
  _mm_store_ps(&a->m10, a0 * _mm_set1_ps(b->m10) + a1 * _mm_set1_ps(b->m11) +
                            a2 * _mm_set1_ps(b->m12) +
                            a3 * _mm_set1_ps(b->m13));
  _mm_store_ps(&a->m20, a0 * _mm_set1_ps(b->m20) + a1 * _mm_set1_ps(b->m21) +
                            a2 * _mm_set1_ps(b->m22) +
                            a3 * _mm_set1_ps(b->m23));
  _mm_store_ps(&a->m30, a0 * _mm_set1_ps(b->m30) + a1 * _mm_set1_ps(b->m31) +
                            a2 * _mm_set1_ps(b->m32) +
                            a3 * _mm_set1_ps(b->m33));
  return a;
}

ct_export CT_Vec4f *ct_mat4f_transform4fv_imm(const CT_Mat4f *m, CT_Vec4f *v) {
  v->mmval = _mm_set1_ps(v->x) * _mm_load_ps(&m->m00) +
             _mm_set1_ps(v->y) * _mm_load_ps(&m->m10) +
             _mm_set1_ps(v->z) * _mm_load_ps(&m->m20) +
             _mm_set1_ps(v->w) * _mm_load_ps(&m->m30);
  return v;
}

ct_export CT_Mat4f *ct_mat4f_scalen_imm(CT_Mat4f *m, float n) {
  __m128 nn = _mm_set_ps(1, n, n, n);
  _mm_store_ps(&m->m00, _mm_load_ps(&m->m00) * nn);
  _mm_store_ps(&m->m10, _mm_load_ps(&m->m10) * nn);
  _mm_store_ps(&m->m20, _mm_load_ps(&m->m20) * nn);
  _mm_store_ps(&m->m30, _mm_load_ps(&m->m30) * nn);
  return m;
}

ct_export CT_Mat4f *ct_mat4f_translate3fp_imm(CT_Mat4f *m, const float *t) {
  _mm_store_ps(&m->m30, _mm_load_ps(&m->m00) * _mm_set1_ps(t[0]) +
                            _mm_load_ps(&m->m10) * _mm_set1_ps(t[1]) +
                            _mm_load_ps(&m->m20) * _mm_set1_ps(t[2]) +
                            _mm_load_ps(&m->m30));
  return m;
}

ct_export CT_Mat4f *ct_mat4f_rotatex_imm(CT_Mat4f *m, float theta) {
  __m128 s  = _mm_set1_ps(sinf(theta));
  __m128 c  = _mm_set1_ps(cosf(theta));
  __m128 t1 = _mm_load_ps(&m->m10);
  __m128 t2 = _mm_load_ps(&m->m20);
  _mm_store_ps(&m->m10, t1 * c + t2 * s);
  _mm_store_ps(&m->m20, t2 * c - t1 * s);
  return m;
}

ct_export CT_Mat4f *ct_mat4f_rotatey_imm(CT_Mat4f *m, float theta) {
  __m128 s  = _mm_set1_ps(sinf(theta));
  __m128 c  = _mm_set1_ps(cosf(theta));
  __m128 t1 = _mm_load_ps(&m->m00);
  __m128 t2 = _mm_load_ps(&m->m20);
  _mm_store_ps(&m->m00, t1 * c - t2 * s);
  _mm_store_ps(&m->m20, t2 * c + t1 * s);
  return m;
}

ct_export CT_Mat4f *ct_mat4f_rotatez_imm(CT_Mat4f *m, float theta) {
  __m128 s  = _mm_set1_ps(sinf(theta));
  __m128 c  = _mm_set1_ps(cosf(theta));
  __m128 t1 = _mm_load_ps(&m->m00);
  __m128 t2 = _mm_load_ps(&m->m10);
  _mm_store_ps(&m->m00, t1 * c + t2 * s);
  _mm_store_ps(&m->m10, t2 * c - t1 * s);
  return m;
}

/*
ct_export CT_Mat4f *ct_mat4f_rotate_axis_imm(CT_Mat4f *m, const CT_Vec3f *axis,
                                             float theta) {
  float c = cosf(theta);
  __m128 a = axis->mmval;
  __m128 s = a * _mm_set1_ps(sinf(theta));
  __m128 si = _mm_setzero_ps() - s;
  __m128 t = a * _mm_set1_ps(1.0f - c);
  __m128 b0 = a * _mm_set1_ps(t[0]) + _mm_set_ps(0, si[1], s[2], c);
  __m128 b1 = a * _mm_set1_ps(t[1]) + _mm_set_ps(0, s[0], c, si[2]);
  __m128 b2 = t * _mm_set1_ps(axis->z) + _mm_set_ps(0, c, si[0], s[1]);
  __m128 a0 = _mm_load_ps(&m->m00);
  __m128 a1 = _mm_load_ps(&m->m10);
  __m128 a2 = _mm_load_ps(&m->m20);
  _mm_store_ps(&m->m00, a0 * _mm_set1_ps(b0[0]) + a1 * _mm_set1_ps(b0[1]) + a2 * _mm_set1_ps(b0[2]));
  _mm_store_ps(&m->m10, a0 * _mm_set1_ps(b1[0]) + a1 * _mm_set1_ps(b1[1]) + a2 * _mm_set1_ps(b1[2]));
  _mm_store_ps(&m->m20, a0 * _mm_set1_ps(b2[0]) + a1 * _mm_set1_ps(b2[1]) + a2 * _mm_set1_ps(b2[2]));
  return m;
}
*/

#else  // END SSE

ct_export CT_Mat4f *ct_mat4f_set_identity(CT_Mat4f *m) {
  memset(m, 0, 16 * sizeof(float));
  m->m00 = m->m11 = m->m22 = m->m33 = 1.0f;
  return m;
}

ct_export CT_Mat4f *ct_mat4f_transpose_imm(CT_Mat4f *m) {
  SWAP(float, m->m01, m->m10);
  SWAP(float, m->m02, m->m20);
  SWAP(float, m->m03, m->m30);
  SWAP(float, m->m12, m->m21);
  SWAP(float, m->m13, m->m31);
  SWAP(float, m->m23, m->m32);
  return m;
}

ct_export CT_Mat4f *ct_mat4f_mul_imm(CT_Mat4f *a, const CT_Mat4f *b) {
  CT_Mat4f t = *a;
  float i = b->m00, j = b->m01, k = b->m02, l = b->m03;
  a->m00 = t.m00 * i + t.m10 * j + t.m20 * k + t.m30 * l;
  a->m01 = t.m01 * i + t.m11 * j + t.m21 * k + t.m31 * l;
  a->m02 = t.m02 * i + t.m12 * j + t.m22 * k + t.m32 * l;
  a->m03 = t.m03 * i + t.m13 * j + t.m23 * k + t.m33 * l;

  i = b->m10, j = b->m11, k = b->m12, l = b->m13;
  a->m10 = t.m00 * i + t.m10 * j + t.m20 * k + t.m30 * l;
  a->m11 = t.m01 * i + t.m11 * j + t.m21 * k + t.m31 * l;
  a->m12 = t.m02 * i + t.m12 * j + t.m22 * k + t.m32 * l;
  a->m13 = t.m03 * i + t.m13 * j + t.m23 * k + t.m33 * l;

  i = b->m20, j = b->m21, k = b->m22, l = b->m23;
  a->m20 = t.m00 * i + t.m10 * j + t.m20 * k + t.m30 * l;
  a->m21 = t.m01 * i + t.m11 * j + t.m21 * k + t.m31 * l;
  a->m22 = t.m02 * i + t.m12 * j + t.m22 * k + t.m32 * l;
  a->m23 = t.m03 * i + t.m13 * j + t.m23 * k + t.m33 * l;

  i = b->m30, j = b->m31, k = b->m32, l = b->m33;
  a->m30 = t.m00 * i + t.m10 * j + t.m20 * k + t.m30 * l;
  a->m31 = t.m01 * i + t.m11 * j + t.m21 * k + t.m31 * l;
  a->m32 = t.m02 * i + t.m12 * j + t.m22 * k + t.m32 * l;
  a->m33 = t.m03 * i + t.m13 * j + t.m23 * k + t.m33 * l;
  return a;
}

ct_export CT_Vec4f *ct_mat4f_transform4fv_imm(const CT_Mat4f *m, CT_Vec4f *v) {
  float x = v->x, y = v->y, z = v->z, w = v->w;
  v->x = x * m->m00 + y * m->m10 + z * m->m20 + w * m->m30;
  v->y = x * m->m01 + y * m->m11 + z * m->m21 + w * m->m31;
  v->z = x * m->m02 + y * m->m12 + z * m->m22 + w * m->m32;
  v->w = x * m->m03 + y * m->m13 + z * m->m23 + w * m->m33;
  return v;
}

ct_export CT_Mat4f *ct_mat4f_scalen_imm(CT_Mat4f *m, float n) {
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

ct_export CT_Mat4f *ct_mat4f_translate3fp_imm(CT_Mat4f *m, const float *t) {
  float x = t[0], y = t[1], z = t[2];
  m->m30 += m->m00 * x + m->m10 * y + m->m20 * z;
  m->m31 += m->m01 * x + m->m11 * y + m->m21 * z;
  m->m32 += m->m02 * x + m->m12 * y + m->m22 * z;
  m->m33 += m->m03 * x + m->m13 * y + m->m23 * z;
  return m;
}

ct_export CT_Mat4f *ct_mat4f_rotatex_imm(CT_Mat4f *m, float theta) {
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

ct_export CT_Mat4f *ct_mat4f_rotatey_imm(CT_Mat4f *m, float theta) {
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

ct_export CT_Mat4f *ct_mat4f_rotatez_imm(CT_Mat4f *m, float theta) {
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

ct_export CT_Mat4f *ct_mat4f_rotate_axis_imm(CT_Mat4f *m,
                                             const CT_Vec3f *axis,
                                             float theta) {
  float x   = axis->x;
  float y   = axis->y;
  float z   = axis->z;
  float s   = sinf(theta);
  float c   = cosf(theta);
  float t   = 1.0f - c;
  float sx  = x * s;
  float sy  = y * s;
  float sz  = z * s;
  float tx  = x * t;
  float ty  = y * t;
  float tz  = z * t;
  float b00 = tx * x + c;
  float b01 = tx * y + sz;
  float b02 = tx * z - sy;
  float b10 = ty * x - sz;
  float b11 = ty * y + c;
  float b12 = ty * z + sx;
  float b20 = tx * z + sy;
  float b21 = ty * z - sx;
  float b22 = tz * z + c;
  float a00 = m->m00, a01 = m->m01, a02 = m->m02, a03 = m->m03;
  float a10 = m->m10, a11 = m->m11, a12 = m->m12, a13 = m->m13;
  float a20 = m->m20, a21 = m->m21, a22 = m->m22, a23 = m->m23;
  m->m00 = a00 * b00 + a10 * b01 + a20 * b02;
  m->m01 = a01 * b00 + a11 * b01 + a21 * b02;
  m->m02 = a02 * b00 + a12 * b01 + a22 * b02;
  m->m03 = a03 * b00 + a13 * b01 + a23 * b02;
  m->m10 = a00 * b10 + a10 * b11 + a20 * b12;
  m->m11 = a01 * b10 + a11 * b11 + a21 * b12;
  m->m12 = a02 * b10 + a12 * b11 + a22 * b12;
  m->m13 = a03 * b10 + a13 * b11 + a23 * b12;
  m->m20 = a00 * b20 + a10 * b21 + a20 * b22;
  m->m21 = a01 * b20 + a11 * b21 + a21 * b22;
  m->m22 = a02 * b20 + a12 * b21 + a22 * b22;
  m->m23 = a03 * b20 + a13 * b21 + a23 * b22;
  return m;
}

ct_export CT_Mat4f *ct_mat4f_transpose(const CT_Mat4f *m, CT_Mat4f *out) {
  *out = *m;
  return ct_mat4f_transpose_imm(out);
}

ct_export CT_Mat4f *ct_mat4f_mul(const CT_Mat4f *a,
                                 const CT_Mat4f *b,
                                 CT_Mat4f *out) {
  *out = *a;
  return ct_mat4f_mul_imm(out, b);
}

ct_export CT_Mat4f *ct_mat4f_scalen(const CT_Mat4f *m, float n, CT_Mat4f *out) {
  *out = *m;
  return ct_mat4f_scalen_imm(out, n);
}

ct_export CT_Mat4f *ct_mat4f_translate3fp(const CT_Mat4f *m,
                                          const float *t,
                                          CT_Mat4f *out) {
  *out = *m;
  return ct_mat4f_translate3fp_imm(out, t);
}

ct_export CT_Mat4f *ct_mat4f_rotatex(const CT_Mat4f *m,
                                     float theta,
                                     CT_Mat4f *out) {
  *out = *m;
  return ct_mat4f_rotatex_imm(out, theta);
}

ct_export CT_Mat4f *ct_mat4f_rotatey(const CT_Mat4f *m,
                                     float theta,
                                     CT_Mat4f *out) {
  *out = *m;
  return ct_mat4f_rotatey_imm(out, theta);
}

ct_export CT_Mat4f *ct_mat4f_rotatez(const CT_Mat4f *m,
                                     float theta,
                                     CT_Mat4f *out) {
  *out = *m;
  return ct_mat4f_rotatez_imm(out, theta);
}

ct_export CT_Mat4f *ct_mat4f_rotate_axis(const CT_Mat4f *m,
                                         const CT_Vec3f *axis,
                                         float theta,
                                         CT_Mat4f *out) {
  *out = *m;
  return ct_mat4f_rotate_axis_imm(out, axis, theta);
}

ct_export CT_Mat4f *ct_mat4f_set_ortho(CT_Mat4f *m,
                                       float l,
                                       float t,
                                       float r,
                                       float b,
                                       float n,
                                       float f) {
  float dx = 1.0f / (l - r);
  float dy = 1.0f / (b - t);
  float dz = 1.0f / (n - f);
  m->m01 = m->m02 = m->m03 = 0;
  m->m10 = m->m12 = m->m13 = 0;
  m->m20 = m->m21 = m->m23 = 0;
  m->m00                   = -2.0f * dx;
  m->m11                   = -2.0f * dy;
  m->m22                   = 2.0f * dz;
  m->m30                   = (l + r) * dx;
  m->m31                   = (t + b) * dy;
  m->m32                   = (n + f) * dz;
  m->m33                   = 1;
  return m;
}

ct_export CT_Mat4f *ct_mat4f_set_perspective(CT_Mat4f *m,
                                             float fovy,
                                             float aspect,
                                             float near,
                                             float far) {
  float f  = 1.0f / tanf(0.5 * fovy * CT_RADIANS);
  float nf = 1.0f / (near - far);
  m->m01 = m->m02 = m->m03 = 0;
  m->m10 = m->m12 = m->m13 = 0;
  m->m20 = m->m21 = 0;
  m->m30 = m->m31 = m->m33 = 0;
  m->m00                   = f / aspect;
  m->m11                   = f;
  m->m22                   = (near + far) * nf;
  m->m23                   = -1;
  m->m32                   = 2 * near * far * nf;
  return m;
}

ct_export int ct_mat4f_set_lookat(CT_Mat4f *m,
                                  const CT_Vec3f *eye,
                                  const CT_Vec3f *target,
                                  const CT_Vec3f *up) {
  CT_Vec3f x, y, z;
  ct_sub3fv(eye, target, &z);
  if (ct_magsq3f(&z) < EPS) {
    return 1;
  }
  ct_normalize3f_imm(&z, 1);
  ct_normalize3f_imm(ct_cross3fv(up, &z, &x), 1);
  ct_normalize3f_imm(ct_cross3fv(&z, &x, &y), 1);
  m->m03 = m->m13 = m->m23 = 0;
  m->m00                   = x.x;
  m->m01                   = y.x;
  m->m02                   = z.x;
  m->m10                   = x.y;
  m->m11                   = y.y;
  m->m12                   = z.y;
  m->m20                   = x.z;
  m->m21                   = y.z;
  m->m22                   = z.z;
  m->m30                   = -ct_dot3fv(&x, eye);
  m->m31                   = -ct_dot3fv(&y, eye);
  m->m32                   = -ct_dot3fv(&z, eye);
  m->m33                   = 1;
  return 0;
}

ct_export void ct_mat4f_trace(const CT_Mat4f *m) {
  CT_INFO("%1.3f %1.3f %1.3f %1.3f", m->m00, m->m10, m->m20, m->m30);
  CT_INFO("%1.3f %1.3f %1.3f %1.3f", m->m01, m->m11, m->m21, m->m31);
  CT_INFO("%1.3f %1.3f %1.3f %1.3f", m->m02, m->m12, m->m22, m->m32);
  CT_INFO("%1.3f %1.3f %1.3f %1.3f", m->m03, m->m13, m->m23, m->m33);
}
