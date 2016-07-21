#pragma once

#include "cthing.h"

#include "math/vec.h"
#include "mem/mem.h"

typedef union {
  struct {
    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;
  };
  float mat[16];
} CT_Mat4f CT_ALIGN(16);

CT_Mat4f *ct_mat4f_set_identity(CT_Mat4f *m);
CT_Mat4f *ct_mat4f_transpose_imm(CT_Mat4f *m);
CT_Mat4f *ct_mat4f_transpose(const CT_Mat4f *m, CT_Mat4f *out);
CT_Mat4f *ct_mat4f_mul_imm(CT_Mat4f *a, const CT_Mat4f *b);
CT_Mat4f *ct_mat4f_mul(const CT_Mat4f *a, const CT_Mat4f *b, CT_Mat4f *out);
CT_Mat4f *ct_mat4f_scalen_imm(CT_Mat4f *m, float n);
CT_Mat4f *ct_mat4f_scalen(const CT_Mat4f *m, float n, CT_Mat4f *out);
CT_Mat4f *ct_mat4f_translate3fp_imm(CT_Mat4f *m, const float *t);
CT_Mat4f *ct_mat4f_translate3fp(const CT_Mat4f *m,
                                const float *t,
                                CT_Mat4f *out);
CT_Mat4f *ct_mat4f_rotatex_imm(CT_Mat4f *m, float theta);
CT_Mat4f *ct_mat4f_rotatex(const CT_Mat4f *m, float theta, CT_Mat4f *out);
CT_Mat4f *ct_mat4f_rotatey_imm(CT_Mat4f *m, float theta);
CT_Mat4f *ct_mat4f_rotatey(const CT_Mat4f *m, float theta, CT_Mat4f *out);
CT_Mat4f *ct_mat4f_rotatez_imm(CT_Mat4f *m, float theta);
CT_Mat4f *ct_mat4f_rotatez(const CT_Mat4f *m, float theta, CT_Mat4f *out);
CT_Mat4f *ct_mat4f_rotate_axis_imm(CT_Mat4f *m,
                                   const CT_Vec3f *axis,
                                   float theta);
CT_Mat4f *ct_mat4f_rotate_axis(const CT_Mat4f *m,
                               const CT_Vec3f *axis,
                               float theta,
                               CT_Mat4f *out);
CT_Vec4f *ct_mat4f_transform4fv_imm(const CT_Mat4f *m, CT_Vec4f *v);

CT_Mat4f *ct_mat4f_set_ortho(CT_Mat4f *m,
                             float l,
                             float t,
                             float r,
                             float b,
                             float n,
                             float f);
CT_Mat4f *ct_mat4f_set_perspective(CT_Mat4f *m,
                                   float fovy,
                                   float aspect,
                                   float near,
                                   float far);
int ct_mat4f_set_lookat(CT_Mat4f *m,
                        const CT_Vec3f *eye,
                        const CT_Vec3f *target,
                        const CT_Vec3f *up);

void ct_mat4f_trace(const CT_Mat4f *m);
