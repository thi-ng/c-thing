#include "data/soa.h"

CT_SOA *ct_soa_new(size_t width, size_t num, size_t stride) {
  void *buf = NULL;
  CT_SOA *s = NULL;
  CT_CHECK(0 == num % (1 << CT_SOA_WORD_SHIFT), "num must be multiple of %d",
           (1 << CT_SOA_WORD_SHIFT));
  s = calloc(1, sizeof(CT_SOA));
  CT_CHECK_MEM(s);
  buf = calloc(width * num, stride);
  CT_CHECK_MEM(buf);
  s->comps = calloc(width, sizeof(void *));
  CT_CHECK_MEM(s->comps);
  for (size_t i = 0; i < width; i++) {
    s->comps[i] = (void *)((uintptr_t)buf + i * num * stride);
  }
  s->num    = num;
  s->width  = width;
  s->stride = stride;
  //CT_INFO("soa: %p, comps: %p, comps[0]: %p", s, s->comps, s->comps[0]);
  return s;
fail:
  if (s) {
    if (buf) {
      free(buf);
    }
    free(s);
  }
  return NULL;
}

int ct_soa_init(CT_SOA *s,
                void **comps,
                size_t width,
                size_t num,
                size_t stride) {
  CT_CHECK(comps, "comps is NULL");
  CT_CHECK(0 == num % (1 << CT_SOA_WORD_SHIFT), "num must be multiple of %d",
           (1 << CT_SOA_WORD_SHIFT));
  s->comps  = comps;
  s->width  = width;
  s->num    = num;
  s->stride = stride;
  return 0;
fail:
  return 1;
}

void ct_soa_free(CT_SOA *s) {
  free(s->comps[0]);
  free(s->comps);
  free(s);
}

void *ct_soa_flatten(const CT_SOA *s) {
  size_t len   = s->num * s->stride;
  uint8_t *out = malloc(s->width * len);
  CT_CHECK_MEM(out);
  for (size_t i = 0; i < s->width; i++) {
    memcpy(&out[i * len], s->comps[i], len);
  }
fail:
  return out;
}

#ifdef CT_FEATURE_SSE

#else

#endif

CT_SOA_PROLOGUE2_SCALAR_IMM(add1f) {
  aa[j] += bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_SCALAR_IMM(sub1f) {
  aa[j] -= bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_SCALAR_IMM(mul1f) {
  aa[j] *= bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_SCALAR_IMM(div1f) {
  aa[j] /= bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_FPTR_IMM(add1fp) {
  aa[j] += bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_FPTR_IMM(sub1fp) {
  aa[j] -= bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_FPTR_IMM(mul1fp) {
  aa[j] *= bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_FPTR_IMM(div1fp) {
  aa[j] /= bb;
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_IMM(add) {
  aa[j] += bb[j];
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_IMM(sub) {
  aa[j] -= bb[j];
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_IMM(mul) {
  aa[j] *= bb[j];
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_IMM(div) {
  aa[j] /= bb[j];
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_IMM(min) {
  aa[j] = CT_SOA_MIN(aa[j], bb[j]);
}
CT_SOA_EPILOGUE_IMM;

CT_SOA_PROLOGUE2_IMM(max) {
  aa[j] = CT_SOA_MAX(aa[j], bb[j]);
}
CT_SOA_EPILOGUE_IMM;

/// immutable SOA ops

CT_SOA_PROLOGUE3(add) {
  oo[j] = aa[j] + bb[j];
}
CT_SOA_EPILOGUE3;

CT_SOA_PROLOGUE3(sub) {
  oo[j] = aa[j] - bb[j];
}
CT_SOA_EPILOGUE3;

CT_SOA_PROLOGUE3(mul) {
  oo[j] = aa[j] * bb[j];
}
CT_SOA_EPILOGUE3;

CT_SOA_PROLOGUE3(div) {
  oo[j] = aa[j] / bb[j];
}
CT_SOA_EPILOGUE3;

//// 2D SOA ops

CT_SOA2_PROLOGUE3(dot2) {
  oo[i] = ax[i] * bx[i] + ay[i] * by[i];
}
CT_SOA_EPILOGUE3_ALT;

CT_SOA2_PROLOGUE3(dist2) {
  ct_soa_vec dx = ax[i] - bx[i];
  ct_soa_vec dy = ay[i] - by[i];
  oo[i]         = CT_SOA_SQRT(dx * dx + dy * dy);
}
CT_SOA_EPILOGUE3_ALT;

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

//// 3D SOA ops

CT_SOA3_PROLOGUE3(dot3) {
  oo[i] = ax[i] * bx[i] + ay[i] * by[i] + az[i] * bz[i];
}
CT_SOA_EPILOGUE3_ALT;
