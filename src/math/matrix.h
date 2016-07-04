#pragma once

#include "cthing.h"
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
CT_Mat4f *ct_mat4f_mul_imm(CT_Mat4f *a, const CT_Mat4f *b);
CT_Mat4f *ct_mat4f_scalen_imm(CT_Mat4f *m, float n);
CT_Mat4f *ct_mat4f_translate3fp_imm(CT_Mat4f *m, const float *t);
CT_Mat4f *ct_mat4f_rotatex_imm(CT_Mat4f *m, float theta);
CT_Mat4f *ct_mat4f_rotatey_imm(CT_Mat4f *m, float theta);
CT_Mat4f *ct_mat4f_rotatez_imm(CT_Mat4f *m, float theta);
