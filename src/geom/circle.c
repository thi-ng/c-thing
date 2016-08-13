#include "geom/circle.h"
#include "geom/protocols.h"

// clang-format off
/*
CT_IArea __ct_iarea_circle2 = {
  .area = ct_circle2f_area
};

CT_ICircumference __ct_icircumference_circle2 = {
  .circumference = ct_circle2f_circumference
};

CT_IClassify __ct_iclassify_circle2 = {
  .classify_point2f = ct_circle2f_classify_point,
  .classify_point3f = NULL
};
*/
// clang-format off

ct_export CT_Circle2f *ct_circle2f_init(CT_Circle2f *c, float x, float y,
                                        float r) {
  c->pos.x = x;
  c->pos.y = y;
  c->r = r;
  return c;
}

ct_export CT_Circle2f *ct_circle2f_initvr(CT_Circle2f *c, CT_Vec2f *p,
                                          float r) {
  return ct_circle2f_init(c, p->x, p->y, r);
}

ct_export CT_Circle2f *ct_circle2f_initr(CT_Circle2f *c, float r) {
  return ct_circle2f_init(c, 0, 0, r);
}

ct_export float ct_circle2f_area(CT_Circle2f *c) {
  return CT_PI * c->r * c->r;
}

ct_export float ct_circle2f_circumference(CT_Circle2f *c) {
  return CT_TAU * c->r;
}

ct_export int ct_circle2f_classify_point(CT_Circle2f *c, CT_Vec2f *p) {
  return ct_signumf(c->r - ct_dist2fv(&c->pos, p), CT_EPS);
}

ct_export CT_Vec2f *ct_circle2f_vertices(CT_Circle2f *c, CT_Vec2f *verts,
                                         size_t n) {
  CT_Vec2f *ptr = verts;
  float t = CT_TAU / n;
  for (size_t i = 0; i < n; i++) {
    ct_add2fv_imm(ct_cartesian2f_imm(ct_set2fxy(ptr, c->r, t * i)), &c->pos);
    ptr++;
  }
  return verts;
}

ct_export CT_Triangle2f *ct_circle2f_tessellate(CT_Circle2f *c,
                                                CT_Triangle2f *tris, size_t n) {
  CT_Vec2f *verts =
      ct_circle2f_vertices(c, malloc(sizeof(CT_Vec2f) * n), n);
  CT_Triangle2f *ptr = tris;
  for (size_t i = 0; i < n; i++) {
    ct_triangle2f_init(ptr, &c->pos, &verts[i], &verts[(i + 1) % n]);
    ptr++;
  }
  free(verts);
  return tris;
}
