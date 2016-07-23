#include "data/soa.h"

CT_SOA_PROLOGUE2_SCALAR(add1f) {
  aa[j] += bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_SCALAR(sub1f) {
  aa[j] -= bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_SCALAR(mul1f) {
  aa[j] *= bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_SCALAR(div1f) {
  aa[j] /= bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_FPTR(add1fp) {
  aa[j] += bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_FPTR(sub1fp) {
  aa[j] -= bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_FPTR(mul1fp) {
  aa[j] *= bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_FPTR(div1fp) {
  aa[j] /= bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2(add) {
  aa[j] += bb[j];
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2(sub) {
  aa[j] -= bb[j];
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2(mul) {
  aa[j] *= bb[j];
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2(div) {
  aa[j] /= bb[j];
}
CT_SOA_EPILOGUE_IMM;

CT_SOA2_PROLOGUE3(dot2) {
  out[i] = ax[i] * bx[i] + ay[i] * by[i];
}
CT_SOA_EPILOGUE3;

CT_SOA3_PROLOGUE3(dot3) {
  out[i] = ax[i] * bx[i] + ay[i] * by[i] + az[i] * bz[i];
}
CT_SOA_EPILOGUE3;

CT_SOA2_PROLOGUE3(dist2) {
  ct_soa_vec dx = ax[i] - bx[i];
  ct_soa_vec dy = ay[i] - by[i];
  out[i]        = CT_SOA_SQRT(dx * dx + dy * dy);
}
CT_SOA_EPILOGUE3;

CT_SOA *ct_soa_normalize2f_imm(CT_SOA *a, float len) {
  size_t n       = a->num >> CT_SOA_WORD_SHIFT;
  ct_soa_vec *ax = (ct_soa_vec *)a->comps[0];
  ct_soa_vec *ay = (ct_soa_vec *)a->comps[1];
  ct_soa_vec ll  = CT_SOA_SET1(len);
  for (size_t i = 0; i < n; i++) {
    ct_soa_vec x = ax[i];
    ct_soa_vec y = ay[i];
    ct_soa_vec l = ll * CT_SOA_RSQRT(x * x + y * y);
    ax[i]        = x * l;
    ay[i]        = y * l;
  }
  return a;
fail:
  return NULL;
}
