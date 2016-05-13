#include "circle.h"

/*
CT_IArea __ct_iarea_circle2 = {.area = ct_circle2f_area};

CT_ICircumference __ct_icircumference_circle2 = {.circumference =
                                                     ct_circle2f_circumference};

CT_IClassify __ct_iclassify_circle2 = {
    .classify_point2f = ct_circle2f_classify_point2f, .classify_point3f = NULL};
*/

CT_EXPORT CT_Circle2f *ct_circle2fxy(float x, float y, float r,
                                     CT_MPool *mpool) {
  CT_Circle2f *c = ALLOCATE_TYPE(mpool, CT_Circle2f);
  if (c != NULL) {
    c->p.x = x;
    c->p.y = y;
    c->r = r;
  }
  return c;
}

CT_EXPORT CT_Circle2f *ct_circle2fv(CT_Vec2f *p, float r, CT_MPool *mpool) {
  return ct_circle2fxy(p->x, p->y, r, mpool);
}

CT_EXPORT CT_Circle2f *ct_circle2fr(float r, CT_MPool *mpool) {
  return ct_circle2fxy(0, 0, r, mpool);
}

CT_EXPORT float ct_circle2f_area(CT_Circle2f *c) {
  return PI * c->r * c->r;
}

CT_EXPORT float ct_circle2f_circumference(CT_Circle2f *c) {
  return TAU * c->r;
}

CT_EXPORT int8_t ct_circle2f_classify_point2f(CT_Circle2f *c, CT_Vec2f *p) {
  return ct_signumf(c->r - ct_dist2fv(&c->p, p), EPS);
}

CT_EXPORT CT_Vec2f *ct_circle2f_vertices(CT_Circle2f *c, CT_Vec2f *verts, uint32_t n) {
  CT_Vec2f *ptr = verts;
  float t = TAU / n;
  for(uint32_t i = 0; i<n; i++) {
    ct_cartesian2f_imm(ct_set2fxy(ptr, c->r, t * i));
    ptr++;
  }
  return verts;
}
